#include <chrono>
#include <csignal>
#include <exception>
#include <future>
#include <iostream>
#include <string>

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

::std::promise<void> g_stop{};

void handler(int) {
    g_stop.set_value();
};

int main() {
    try {
        ::std::signal(SIGTERM, &handler);
        ::std::signal(SIGINT, &handler);

        init_meter();
        init_tracer();
        init_logger();

        ::std::chrono::milliseconds timeout{ 30 * 1000 };
        ::std::string auth_address{ "https://id.twitch.tv/oauth2/" };
        ::std::string client_id{ "client_id" };
        ::std::string client_secret{ "client_secret" };
        ::std::string refresh_token{ "refresh_token" };
        ::std::string irc_address{ "wss://irc-ws.chat.twitch.tv" };
        ::std::string botname{ "botname" };
        ::std::string openai_address{ "https://generativelanguage.googleapis.com/v1beta/openai/" };
        ::std::string model{ "gemini-2.0-flash" };
        ::std::string key{ "key" };

        ::ai::chat::clients::twitch::auth auth{ auth_address, timeout };
        ::ai::chat::clients::twitch::irc<::ai::chat::clients::twitch::handlers::observable> irc{ irc_address, timeout };
        ::ai::chat::adapters::openai openai{ openai_address, timeout };
        ::ai::chat::histories::observable<::ai::chat::histories::sqlite> sqlite{ botname };
        auto irc_binding = ::ai::chat::binders::twitch<decltype(sqlite), decltype(irc)>::bind(sqlite, irc);
        auto openai_binding = ::ai::chat::binders::openai<decltype(sqlite), decltype(openai)>::bind(sqlite, openai,
            botname, model, key);

        ::ai::chat::clients::twitch::token_context access_context{ auth.refresh_token(client_id, client_secret, refresh_token) };
        irc.connect(botname, access_context.access_token);
        g_stop.get_future().wait();
        irc.disconnect();
    }
    catch(const ::std::exception& e) {
        ::std::cerr << "Error: " << e.what() << ::std::endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}