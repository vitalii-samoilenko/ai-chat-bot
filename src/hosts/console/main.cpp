#include <chrono>
#include <exception>
#include <iostream>
#include <vector>

#include "ai/chat/adapters/rate_limited.hpp"
#include "ai/chat/adapters/tokens_limited.hpp"
#include "ai/chat/adapters/openai.hpp"
#include "ai/chat/adapters/log.hpp"
#include "ai/chat/adapters/content_length.hpp"
#include "ai/chat/adapters/total_tokens.hpp"
#include "ai/chat/adapters/trace.hpp"
#include "twitch/auth/client.hpp"
#include "twitch/irc/client.hpp"

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

int main() {
    try {
        init_meter();
        init_tracer();
        init_logger();

        ::ai::chat::history history{};
        ::std::vector<::openai::message> messages{
            {
                ::openai::role::system,
                "Message must not be longer than 200 symbols"
            }
        };
        ::ai::chat::adapters::log<
        ::ai::chat::adapters::content_length<
        ::ai::chat::adapters::total_tokens<
        ::ai::chat::adapters::trace<
        ::ai::chat::adapters::rate_limited<
        ::ai::chat::adapters::tokens_limited<
        ::ai::chat::adapters::openai<::std::vector<::openai::message>>
        >>>>>> adapter{
            1,
            history, 999000, ::std::chrono::nanoseconds{ 24 * 60 * 60 * 1000000000 },
            "gemini-2.0-flash", messages,
            "https://generativelanguage.googleapis.com/v1beta/openai/",
            "api_key",
            ::std::chrono::milliseconds{ 30 * 1000 }
        };

        ::twitch::auth::client auth_client {
            "https://id.twitch.tv/oauth2/",
            ::std::chrono::milliseconds{ 30 * 1000 }
        };
        ::twitch::auth::access_context access_context{
            "access_token",
            "refresh_token"
        };

        ::twitch::irc::client irc_client {
            "wss://irc-ws.chat.twitch.tv",
            ::std::chrono::milliseconds{ 30 * 1000 }
        };
        ::std::vector<::std::string> channels{ "botname" };

        ::twitch::irc::subscription& subscription{ irc_client.subscribe<decltype(adapter)>() };
        subscription.on_message([&](::twitch::irc::context& context, const ::twitch::irc::message& message)->void {
            messages.push_back({ ::openai::role::user, message.content });
            ::std::pair<::std::string, size_t> result{ adapter.complete() };
            messages.push_back({ ::openai::role::assistant, result.first });
            context.send({ "", message.channel, result.first });
        });

        do {
            if (!auth_client.validate_token(access_context.access_token)) {
                access_context = auth_client.refresh_token(
                    "client_id", "client_secret",
                    access_context.refresh_token);
            }
        }
        while (irc_client.run("botname", access_context.access_token, channels));
    }
    catch(const ::std::exception& e) {
        ::std::cerr << "Error: " << e.what() << ::std::endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}