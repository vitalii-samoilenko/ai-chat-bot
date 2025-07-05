#include <chrono>
#include <stdexcept>
#include <fstream>
#include <iostream>
#include <string>
#include <utility>
#include <vector>

#include "boost/json.hpp"

#include "ai/chat/clients/auth.hpp"
#include "ai/chat/clients/twitch.hpp"
#include "ai/chat/clients/observable.hpp"
#include "ai/chat/adapters/openai.hpp"
#include "ai/chat/histories/sqlite.hpp"
#include "ai/chat/histories/observable.hpp"
#include "ai/chat/moderators/sqlite.hpp"
#include "ai/chat/binders/twitch.hpp"
#include "ai/chat/binders/openai.hpp"
#include "ai/chat/commands/join.hpp"
#include "ai/chat/commands/executor.hpp"

#include "opentelemetry/exporters/ostream/metric_exporter_factory.h"
#include "opentelemetry/sdk/metrics/export/periodic_exporting_metric_reader_factory.h"
#include "opentelemetry/sdk/metrics/meter_context_factory.h"
#include "opentelemetry/sdk/metrics/meter_provider_factory.h"
#include "opentelemetry/metrics/provider.h"

#include "opentelemetry/exporters/ostream/span_exporter_factory.h"
#include "opentelemetry/sdk/trace/simple_processor_factory.h"
#include "opentelemetry/sdk/trace/tracer_provider_factory.h"
#include "opentelemetry/trace/provider.h"

#include "opentelemetry/exporters/ostream/log_record_exporter_factory.h"
#include "opentelemetry/sdk/logs/simple_log_record_processor_factory.h"
#include "opentelemetry/sdk/logs/logger_provider_factory.h"
#include "opentelemetry/logs/provider.h"

void init_meter() {
    auto exporter = ::opentelemetry::exporter::metrics::OStreamMetricExporterFactory::Create();
    auto reader = ::opentelemetry::sdk::metrics::PeriodicExportingMetricReaderFactory::Create(::std::move(exporter),
        ::opentelemetry::sdk::metrics::PeriodicExportingMetricReaderOptions{
            ::std::chrono::milliseconds{ 1000 },
            ::std::chrono::milliseconds{ 500 }
        });
    auto context = ::opentelemetry::sdk::metrics::MeterContextFactory::Create();
    context->AddMetricReader(::std::move(reader));
    auto sdk_provider = ::opentelemetry::sdk::metrics::MeterProviderFactory::Create(::std::move(context));
    ::std::shared_ptr<::opentelemetry::metrics::MeterProvider> api_provider{ ::std::move(sdk_provider) };
    ::opentelemetry::metrics::Provider::SetMeterProvider(api_provider);
};
void init_tracer() {
    auto exporter  = ::opentelemetry::exporter::trace::OStreamSpanExporterFactory::Create();
    auto processor = ::opentelemetry::sdk::trace::SimpleSpanProcessorFactory::Create(std::move(exporter));
    auto sdk_provider = ::opentelemetry::sdk::trace::TracerProviderFactory::Create(std::move(processor));
    ::std::shared_ptr<::opentelemetry::trace::TracerProvider> api_provider{ ::std::move(sdk_provider) };
    ::opentelemetry::trace::Provider::SetTracerProvider(api_provider);
};
void init_logger() {
    auto exporter = ::opentelemetry::exporter::logs::OStreamLogRecordExporterFactory::Create();
    auto processor = ::opentelemetry::sdk::logs::SimpleLogRecordProcessorFactory::Create(::std::move(exporter));
    auto sdk_provider = ::opentelemetry::sdk::logs::LoggerProviderFactory::Create(::std::move(processor));
    ::std::shared_ptr<::opentelemetry::logs::LoggerProvider> api_provider{ ::std::move(sdk_provider) };
    ::opentelemetry::logs::Provider::SetLoggerProvider(api_provider);
};
::std::string botname{};
::std::vector<::std::string> moderators{};
::std::vector<::std::string> allowed{};
::std::vector<::std::pair<::std::string, ::std::string>> filters{};
size_t retries{};
::std::string apology{};
::std::string pattern{};
::std::vector<::ai::chat::histories::message> context{};
::std::chrono::milliseconds auth_timeout{};
::std::string auth_address{};
::std::string auth_client_id{};
::std::string auth_client_secret{};
::std::string auth_refresh_token{};
::std::string client_address{};
::std::chrono::milliseconds client_timeout{};
::std::chrono::milliseconds client_delay{};
::std::string adapter_address{};
::std::chrono::milliseconds adapter_timeout{};
::std::chrono::milliseconds adapter_delay{};
::std::string adapter_model{};
::std::string adapter_key{};
::std::string history_filename{};
::std::string moderator_filename{};
size_t moderator_length{};
void init_config(const ::std::string& filename) {
    ::std::ifstream input{ filename };
    if (!input.is_open()) {
        throw ::std::invalid_argument{ "configuration not found" };
    }
    ::boost::json::monotonic_resource buffer{};
    ::boost::json::stream_parser parser{ &buffer };
    ::std::string line{};
    while (::std::getline(input, line)) {
        parser.write_some(line);
    }
    ::boost::json::value config{ parser.release() };
    ::boost::json::value& client{ config.at("client") };
    ::boost::json::value& auth{ client.at("auth") };
    ::boost::json::value& history{ config.at("history") };
    ::boost::json::value& adapter{ config.at("adapter") };
    ::boost::json::value& moderator{ config.at("moderator") };
    auth_address = auth.at("address").as_string();
    auth_timeout = ::std::chrono::milliseconds{ auth.at("timeout").as_int64() };
    auth_client_id = auth.at("client_id").as_string();
    auth_client_secret = auth.at("client_secret").as_string();
    auth_refresh_token = auth.at("refresh_token").as_string();
    client_address = client.at("address").as_string();
    client_timeout = ::std::chrono::milliseconds{ client.at("timeout").as_int64() };
    client_delay = ::std::chrono::milliseconds{ client.at("delay").as_int64() };
    history_filename = history.at("filename").as_string();
    adapter_address = adapter.at("address").as_string();
    adapter_timeout = ::std::chrono::milliseconds{ adapter.at("timeout").as_int64() };
    adapter_delay = ::std::chrono::milliseconds{ adapter.at("delay").as_int64() };
    adapter_model = adapter.at("model").as_string();
    adapter_key = adapter.at("key").as_string();
    moderator_filename = moderator.at("filename").as_string();
    moderator_length = static_cast<size_t>(moderator.at("length").as_int64());
    botname = config.at("botname").as_string();
    retries = static_cast<size_t>(config.at("retries").as_int64());
    apology = config.at("apology").as_string();
    pattern = config.at("pattern").as_string();
    ::std::ifstream history_file{ history_filename };
    if (!history_file.is_open()) {
        for (::boost::json::value& config_message : config.at("context").as_array()) {
            ::ai::chat::histories::message history_message{};
            for (::boost::json::value& config_tag : config_message.at("tags").as_array()) {
                ::ai::chat::histories::tag history_tag{};
                history_tag.name = config_tag.at("name").as_string();
                history_tag.value = config_tag.at("value").as_string();
                history_message.tags.push_back(::std::move(history_tag));
            }
            history_message.content = config_message.at("content").as_string();
            context.push_back(::std::move(history_message));
        }
    }
    ::std::ifstream moderator_file{ moderator_filename };
    if (!moderator_file.is_open()) {
        for (::boost::json::value& config_username : config.at("moderators").as_array()) {
            ::std::string moderator_username{ config_username.as_string() };
            moderators.push_back(::std::move(moderator_username));
        }
        for (::boost::json::value& config_username : config.at("allowed").as_array()) {
            ::std::string moderator_username{ config_username.as_string() };
            allowed.push_back(::std::move(moderator_username));
        }
        for (::boost::json::value& config_filter : config.at("filters").as_array()) {
            ::std::pair<::std::string, ::std::string> moderator_filter{
                config_filter.at("name").as_string(),
                config_filter.at("pattern").as_string()
            };
            filters.push_back(::std::move(moderator_filter));
        }
    }
};

int main(int argc, char* argv[]) {
    try {
        init_meter();
        init_tracer();
        init_logger();
        init_config(argc == 2
            ? argv[1]
            : "config.json");

        ::ai::chat::clients::auth auth{ auth_address, auth_timeout };
        ::ai::chat::clients::observable<::ai::chat::clients::twitch> client{ client_address, client_timeout, client_delay, 0 };
        ::ai::chat::adapters::openai adapter{ adapter_address, adapter_timeout, adapter_delay };
        ::ai::chat::histories::observable<::ai::chat::histories::sqlite> history{ history_filename };
        ::ai::chat::moderators::sqlite moderator{ moderator_filename, moderator_length };
        ::ai::chat::commands::executor<
        ::ai::chat::commands::join<decltype(client)>
        > executor{ client };

        for (const ::std::string& username : moderators) {
            moderator.mod(username);
        }
        for (const ::std::string& username : allowed) {
            moderator.allow(username);
        }
        for (const ::std::pair<::std::string, ::std::string>& name_n_pattern : filters) {
            moderator.filter(name_n_pattern.first, name_n_pattern.second);
        }
        for (const ::ai::chat::histories::message& history_message : context) {
            history.insert<decltype(main)>(history_message);
            ::ai::chat::adapters::message adapter_message{
                ::ai::chat::adapters::role::system,
                history_message.content
            };
            for (const ::ai::chat::histories::tag& tag : history_message.tags) {
                if (tag.name == "user.name") {
                    adapter_message.role = tag.value == botname
                        ? ::ai::chat::adapters::role::assistant
                        : ::ai::chat::adapters::role::user;
                }
            }
            adapter.insert(adapter_message);
        }

        auto client_binding = ::ai::chat::binders::twitch<decltype(history), decltype(client)>::bind(history, client,
            moderator, executor,
            botname);
        auto adapter_binding = ::ai::chat::binders::openai<decltype(history), decltype(adapter)>::bind(history, adapter,
            moderator,
            adapter_model, adapter_key,
            pattern, retries, apology,
            botname);

        ::ai::chat::clients::token_context access_context{ auth.refresh_token(auth_client_id, auth_client_secret, auth_refresh_token) };
        client.connect(botname, access_context.access_token);
        ::std::cout << "Waiting for shutdown signal..." << ::std::endl;
        client.attach();
    }
    catch(const ::std::exception& e) {
        ::std::cerr << "Error: " << e.what() << ::std::endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
};
