#include <fstream>
#include <iostream>
#include <exception>
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
#include "ai/chat/commands.hpp"

#include "opentelemetry/exporters/otlp/otlp_grpc_log_record_exporter_factory.h"
#include "opentelemetry/sdk/logs/simple_log_record_processor_factory.h"
#include "opentelemetry/sdk/logs/logger_provider_factory.h"
#include "opentelemetry/logs/provider.h"

#include "opentelemetry/exporters/otlp/otlp_grpc_metric_exporter_factory.h"
#include "opentelemetry/sdk/metrics/export/periodic_exporting_metric_reader_factory.h"
#include "opentelemetry/sdk/metrics/view/view_registry_factory.h"
#include "opentelemetry/sdk/metrics/meter_context_factory.h"
#include "opentelemetry/sdk/metrics/meter_provider_factory.h"
#include "opentelemetry/metrics/provider.h"

#include "opentelemetry/exporters/otlp/otlp_grpc_exporter_factory.h"
#include "opentelemetry/sdk/trace/simple_processor_factory.h"
#include "opentelemetry/sdk/trace/tracer_provider_factory.h"
#include "opentelemetry/trace/provider.h"

#include "eboost/system/cpu.hpp"
#include "eboost/system/memory.hpp"

#include "sqlite3.h"

::boost::json::monotonic_resource buffer{};
::boost::json::value config{};
void init_config(::std::string_view filename) {
    ::std::ifstream input{ filename.data() };
    if (!input.is_open()) {
        throw ::std::invalid_argument{ "configuration not found" };
    }
    ::boost::json::stream_parser parser{ &buffer };
    ::std::string line{};
    while (::std::getline(input, line)) {
        parser.write_some(line);
    }
    config = parser.release();
};

void init_logger(::std::string_view endpoint) {
    ::opentelemetry::exporter::otlp::OtlpGrpcLogRecordExporterOptions options{};
    options.endpoint = endpoint;
    auto exporter = ::opentelemetry::exporter::otlp::OtlpGrpcLogRecordExporterFactory::Create(options);
    auto processor = ::opentelemetry::sdk::logs::SimpleLogRecordProcessorFactory::Create(::std::move(exporter));
    auto resource = ::opentelemetry::sdk::resource::Resource::Create(
        {
            {"service.name", "ai_chat_hosts_console"}
        });
    auto sdk_provider = ::opentelemetry::sdk::logs::LoggerProviderFactory::Create(::std::move(processor), resource);
    ::std::shared_ptr<::opentelemetry::logs::LoggerProvider> api_provider{ ::std::move(sdk_provider) };
    ::opentelemetry::logs::Provider::SetLoggerProvider(api_provider);
};
void init_meter(::std::string_view endpoint) {
    ::opentelemetry::exporter::otlp::OtlpGrpcMetricExporterOptions options{};
    options.endpoint = endpoint;
    auto exporter = ::opentelemetry::exporter::otlp::OtlpGrpcMetricExporterFactory::Create(options);
    auto reader = ::opentelemetry::sdk::metrics::PeriodicExportingMetricReaderFactory::Create(::std::move(exporter),
        ::opentelemetry::sdk::metrics::PeriodicExportingMetricReaderOptions{
            ::std::chrono::milliseconds{ 1000 },
            ::std::chrono::milliseconds{ 500 }
        });
    auto views = ::opentelemetry::sdk::metrics::ViewRegistryFactory::Create();
    auto resource = ::opentelemetry::sdk::resource::Resource::Create(
        {
            {"service.name", "ai_chat_hosts_console"}
        });
    auto context = ::opentelemetry::sdk::metrics::MeterContextFactory::Create(::std::move(views), resource);
    context->AddMetricReader(::std::move(reader));
    auto sdk_provider = ::opentelemetry::sdk::metrics::MeterProviderFactory::Create(::std::move(context));
    ::std::shared_ptr<::opentelemetry::metrics::MeterProvider> api_provider{ ::std::move(sdk_provider) };
    ::opentelemetry::metrics::Provider::SetMeterProvider(api_provider);
};
void init_tracer(::std::string_view endpoint) {
    ::opentelemetry::exporter::otlp::OtlpGrpcExporterOptions options{};
    options.endpoint = endpoint;
    auto exporter  = ::opentelemetry::exporter::otlp::OtlpGrpcExporterFactory::Create(options);
    auto processor = ::opentelemetry::sdk::trace::SimpleSpanProcessorFactory::Create(std::move(exporter));
    auto resource = ::opentelemetry::sdk::resource::Resource::Create(
        {
            {"service.name", "ai_chat_hosts_console"}
        });
    auto sdk_provider = ::opentelemetry::sdk::trace::TracerProviderFactory::Create(std::move(processor), resource);
    ::std::shared_ptr<::opentelemetry::trace::TracerProvider> api_provider{ ::std::move(sdk_provider) };
    ::opentelemetry::trace::Provider::SetTracerProvider(api_provider);
};

void on_cpu_total(::opentelemetry::metrics::ObserverResult observer_result, void *state) {
    auto usage = ::eboost::system::cpu::get_usage();
    auto gauge = ::opentelemetry::nostd::get<
        ::opentelemetry::nostd::shared_ptr<opentelemetry::metrics::ObserverResultT<int64_t>>>(
        observer_result);
    gauge->Observe(usage.system, {
        {"mode", "system"}
    });
    gauge->Observe(usage.user, {
        {"mode", "user"}
    });
};
void on_memory_total(::opentelemetry::metrics::ObserverResult observer_result, void *state) {
    auto usage = ::eboost::system::memory::get_usage();
    auto gauge = ::opentelemetry::nostd::get<
        ::opentelemetry::nostd::shared_ptr<opentelemetry::metrics::ObserverResultT<int64_t>>>(
        observer_result);
    gauge->Observe(usage.anonymous, {
        {"type", "anonymous"}
    });
    gauge->Observe(usage.shared, {
        {"type", "shared"}
    });
};

int main(int argc, char* argv[]) {
    try {
        init_config(argc == 2
            ? argv[1]
            : "config.json");

        init_logger(config.at("telemetry").at("endpoint").as_string());
        init_meter(config.at("telemetry").at("endpoint").as_string());
        init_tracer(config.at("telemetry").at("endpoint").as_string());
        auto meter = ::opentelemetry::metrics::Provider::GetMeterProvider()
            ->GetMeter("ai_chat_hosts_console");
        auto m_cpu_total = meter->CreateInt64ObservableGauge("ai_chat_hosts_console_cpu_total");
        m_cpu_total->AddCallback(on_cpu_total, nullptr);
        auto m_memory_total = meter->CreateInt64ObservableGauge("ai_chat_hosts_console_memory_total");
        m_memory_total->AddCallback(on_memory_total, nullptr);

        auto on_exit = ::boost::scope::make_scope_exit([]()->void {
            ::sqlite3_shutdown();
        });
        ::sqlite3_initialize();

        bool history_exists{ false };
        {
            ::std::ifstream file{ config.at("history").at("filename").as_string().c_str() };
            history_exists = file.is_open();
        }
        ::ai::chat::histories::observable<::ai::chat::histories::sqlite> history{
            config.at("history").at("filename").as_string()
        };
        if (!history_exists) {
            for (::boost::json::value const &message : config.at("context").as_array()) {
                ::std::vector<::ai::chat::histories::tag> tags{};
                for (::boost::json::value const &tag : message.at("tags").as_array()) {
                    tags.emplace_back(
                        tag.at("name").as_string(),
                        tag.at("value").as_string());
                }
                history.insert<decltype(main)>(::ai::chat::histories::message{
                    {},
                    message.at("content").as_string(),
                    tags
                });
            }
        }

        ::ai::chat::adapters::openai adapter{
            config.at("adapter").at("address").as_string(),
            ::std::chrono::milliseconds{ config.at("adapter").at("timeout").as_int64() },
            ::std::chrono::milliseconds{ config.at("adapter").at("delay").as_int64() },
            static_cast<size_t>(config.at("adapter").at("limit").as_int64())
        };
        {
            ::ai::chat::histories::observable_iterator<::ai::chat::histories::sqlite> current{ history.begin() };
            ::ai::chat::histories::observable_iterator<::ai::chat::histories::sqlite> last{ history.end() };
            adapter.reserve(static_cast<size_t>(last - current));
            for (; !(current == last); ++current) {
                ::ai::chat::histories::message message{ *current };
                ::ai::chat::histories::tag const *username_tag{ nullptr };
                for (::ai::chat::histories::tag const &tag : message.tags) {
                    if (tag.name == "user.name") {
                        username_tag = &tag;
                        break;
                    }
                }
                adapter.push_back(::ai::chat::adapters::message{
                    username_tag
                        ? username_tag->value == config.at("botname").as_string()
                            ? ::ai::chat::adapters::role::assistant
                            : ::ai::chat::adapters::role::user
                        : ::ai::chat::adapters::role::system,
                    message.content
                });
            }
        }

        ::ai::chat::clients::auth auth{
            config.at("client").at("auth").at("address").as_string(),
            ::std::chrono::milliseconds{ config.at("client").at("auth").at("timeout").as_int64() },
        };
        ::ai::chat::clients::observable<::ai::chat::clients::twitch> client{
            config.at("client").at("address").as_string(),
            ::std::chrono::milliseconds{ config.at("client").at("timeout").as_int64() },
            ::std::chrono::milliseconds{ config.at("client").at("delay").as_int64() },
            0
        };

        bool moderator_exists{ false };
        {
            ::std::ifstream file{ config.at("moderator").at("filename").as_string().c_str() };
            moderator_exists = file.is_open();
        }
        ::ai::chat::moderators::sqlite moderator{
            config.at("moderator").at("filename").as_string(),
            static_cast<size_t>(config.at("moderator").at("length").as_int64()),
        };
        if (!moderator_exists) {
            for (::boost::json::value const &username : config.at("administrators").as_array()) {
                moderator.admin(username.as_string());
            }
            for (::boost::json::value const &username : config.at("allowed").as_array()) {
                moderator.allow(username.as_string());
            }
            for (::boost::json::value const &filter : config.at("filters").as_array()) {
                moderator.filter(
                    filter.at("name").as_string(),
                    filter.at("pattern").as_string()
                );
            }
        }

        ::ai::chat::commands::executor<
        ::ai::chat::commands::allow<::ai::chat::moderators::sqlite>,
        ::ai::chat::commands::ban<::ai::chat::moderators::sqlite>,
        ::ai::chat::commands::content<::ai::chat::histories::sqlite>,
        ::ai::chat::commands::deny<::ai::chat::moderators::sqlite>,
        ::ai::chat::commands::edit<::ai::chat::histories::sqlite>,
        ::ai::chat::commands::find<::ai::chat::histories::sqlite, 25>,
        ::ai::chat::commands::instruct<::ai::chat::histories::sqlite>,
        ::ai::chat::commands::join<::ai::chat::clients::twitch>,
        ::ai::chat::commands::leave<::ai::chat::clients::twitch>,
        ::ai::chat::commands::mod<::ai::chat::moderators::sqlite>,
        ::ai::chat::commands::remove<::ai::chat::histories::sqlite>,
        ::ai::chat::commands::timeout<::ai::chat::moderators::sqlite>,
        ::ai::chat::commands::unban<::ai::chat::moderators::sqlite>,
        ::ai::chat::commands::unmod<::ai::chat::moderators::sqlite>
        > executor{
            moderator,
            moderator,
            history,
            moderator,
            history,
            history,
            history,
            client,
            client,
            moderator,
            history,
            moderator,
            moderator,
            moderator
        };

        auto client_binding = ::ai::chat::binders::twitch<::ai::chat::histories::sqlite>::bind(history, client,
            moderator, executor,
            config.at("botname").as_string());
        auto adapter_binding = ::ai::chat::binders::openai<::ai::chat::histories::sqlite>::bind(history, adapter,
            moderator,
            config.at("adapter").at("model").as_string(), config.at("adapter").at("key").as_string(),
            static_cast<size_t>(config.at("adapter").at("skip").as_int64()), ::std::chrono::hours{ config.at("adapter").at("range").as_int64() },
            config.at("pattern").as_string(), static_cast<size_t>(config.at("retries").as_int64()), config.at("apology").as_string(),
            config.at("botname").as_string());

        ::ai::chat::clients::token_context access_context{
            auth.refresh_token(
                config.at("client").at("auth").at("client_id").as_string(),
                config.at("client").at("auth").at("client_secret").as_string(),
                config.at("client").at("auth").at("refresh_token").as_string())
        };
        client.connect(
            config.at("botname").as_string(),
            access_context.access_token);
        config.emplace_null();
        buffer.release();
        ::std::cout << "Waiting for shutdown signal..." << ::std::endl;
        client.attach();
    }
    catch(const ::std::exception& e) {
        ::std::cerr << "Error: " << e.what() << ::std::endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
};
