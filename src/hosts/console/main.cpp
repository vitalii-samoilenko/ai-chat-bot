#include <chrono>
#include <stdexcept>
#include <fstream>
#include <iostream>
#include <string>

#include "boost/json.hpp"

#include "ai/chat/clients/twitch/auth.hpp"
#include "ai/chat/clients/twitch/irc.hpp"
#include "ai/chat/clients/twitch/handlers/observable.hpp"
#include "ai/chat/adapters/openai.hpp"
#include "ai/chat/histories/sqlite.hpp"
#include "ai/chat/histories/observable.hpp"
#include "ai/chat/binders/twitch.hpp"
#include "ai/chat/binders/openai.hpp"

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
::std::string auth_address{};
::std::chrono::milliseconds auth_timeout{};
::std::string auth_client_id{};
::std::string auth_client_secret{};
::std::string auth_refresh_token{};
::std::string irc_address{};
::std::chrono::milliseconds irc_timeout{};
::std::string irc_botname{};
::std::string openai_address{};
::std::chrono::milliseconds openai_timeout{};
::std::string openai_model{};
::std::string openai_key{};
::std::string sqlite_filename{};
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
    ::boost::json::value& irc{ client.at("irc") };
    ::boost::json::value& sqlite{ config.at("history") };
    ::boost::json::value& openai{ config.at("adapter") };
    auth_address.append(auth.at("address").as_string());
    auth_timeout = ::std::chrono::milliseconds{ auth.at("timeout").as_int64() };
    auth_client_id.append(auth.at("client_id").as_string());
    auth_client_secret.append(auth.at("client_secret").as_string());
    auth_refresh_token.append(auth.at("refresh_token").as_string());
    irc_address.append(irc.at("address").as_string());
    irc_timeout = ::std::chrono::milliseconds{ irc.at("timeout").as_int64() };
    irc_botname.append(irc.at("botname").as_string());
    openai_address.append(openai.at("address").as_string());
    openai_timeout = ::std::chrono::milliseconds{ openai.at("timeout").as_int64() };
    openai_model.append(openai.at("model").as_string());
    openai_key.append(openai.at("key").as_string());
    sqlite_filename.append(sqlite.at("filename").as_string());
};

int main(int argc, char* argv[]) {
    try {
        init_meter();
        init_tracer();
        init_logger();
        init_config(argc
            ? argv[0]
            : "config.json");

        ::ai::chat::clients::twitch::auth auth{ auth_address, auth_timeout };
        ::ai::chat::clients::twitch::irc<::ai::chat::clients::twitch::handlers::observable> irc{ 0, irc_address, irc_timeout };
        ::ai::chat::adapters::openai openai{ openai_address, openai_timeout };
        ::ai::chat::histories::observable<::ai::chat::histories::sqlite> sqlite{ sqlite_filename };
        auto irc_binding = ::ai::chat::binders::twitch<decltype(sqlite), decltype(irc)>::bind(sqlite, irc);
        auto openai_binding = ::ai::chat::binders::openai<decltype(sqlite), decltype(openai)>::bind(sqlite, openai,
            irc_botname, openai_model, openai_key);

        ::ai::chat::clients::twitch::token_context access_context{ auth.refresh_token(auth_client_id, auth_client_secret, auth_refresh_token) };
        irc.connect(irc_botname, access_context.access_token);
        ::std::cout << "Waiting for shutdown signal..." << ::std::endl;
        irc.attach();
    }
    catch(const ::std::exception& e) {
        ::std::cerr << "Error: " << e.what() << ::std::endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
};
