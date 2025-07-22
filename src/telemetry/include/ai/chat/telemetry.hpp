#ifndef AI_CHAT_TELEMETRY_HPP
#define AI_CHAT_TELEMETRY_HPP

#ifdef ENABLE_TELEMETRY

#include <utility>

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

#include "eboost/system.hpp"

#define SYSTEM_CALLBACKS()\
void on_cpu_total(::opentelemetry::metrics::ObserverResult observer_result, void *state) {\
    auto usage = ::eboost::system::cpu::get_usage();\
    auto gauge = ::opentelemetry::nostd::get<\
        ::opentelemetry::nostd::shared_ptr<opentelemetry::metrics::ObserverResultT<int64_t>>>(\
        observer_result);\
    gauge->Observe(usage.system, {\
        {"mode", "system"}\
    });\
    gauge->Observe(usage.user, {\
        {"mode", "user"}\
    });\
};\
\
void on_memory_total(::opentelemetry::metrics::ObserverResult observer_result, void *state) {\
    auto usage = ::eboost::system::memory::get_usage();\
    auto gauge = ::opentelemetry::nostd::get<\
        ::opentelemetry::nostd::shared_ptr<opentelemetry::metrics::ObserverResultT<int64_t>>>(\
        observer_result);\
    gauge->Observe(usage.anonymous, {\
        {"type", "anonymous"}\
    });\
    gauge->Observe(usage.shared, {\
        {"type", "shared"}\
    });\
};
#define INIT_TELEMETRY(endpoint, service_name)\
{\
    ::opentelemetry::exporter::otlp::OtlpGrpcLogRecordExporterOptions options{};\
    options.endpoint = endpoint;\
    auto exporter = ::opentelemetry::exporter::otlp::OtlpGrpcLogRecordExporterFactory::Create(options);\
    auto processor = ::opentelemetry::sdk::logs::SimpleLogRecordProcessorFactory::Create(::std::move(exporter));\
    auto resource = ::opentelemetry::sdk::resource::Resource::Create(\
        {\
            {"service.name", service_name}\
        });\
    auto sdk_provider = ::opentelemetry::sdk::logs::LoggerProviderFactory::Create(::std::move(processor), resource);\
    ::std::shared_ptr<::opentelemetry::logs::LoggerProvider> api_provider{ ::std::move(sdk_provider) };\
    ::opentelemetry::logs::Provider::SetLoggerProvider(api_provider);\
}\
{\
    ::opentelemetry::exporter::otlp::OtlpGrpcMetricExporterOptions options{};\
    options.endpoint = endpoint;\
    auto exporter = ::opentelemetry::exporter::otlp::OtlpGrpcMetricExporterFactory::Create(options);\
    auto reader = ::opentelemetry::sdk::metrics::PeriodicExportingMetricReaderFactory::Create(::std::move(exporter),\
        ::opentelemetry::sdk::metrics::PeriodicExportingMetricReaderOptions{\
            ::std::chrono::milliseconds{ 1000 },\
            ::std::chrono::milliseconds{ 500 }\
        });\
    auto views = ::opentelemetry::sdk::metrics::ViewRegistryFactory::Create();\
    auto resource = ::opentelemetry::sdk::resource::Resource::Create(\
        {\
            {"service.name", service_name}\
        });\
    auto context = ::opentelemetry::sdk::metrics::MeterContextFactory::Create(::std::move(views), resource);\
    context->AddMetricReader(::std::move(reader));\
    auto sdk_provider = ::opentelemetry::sdk::metrics::MeterProviderFactory::Create(::std::move(context));\
    ::std::shared_ptr<::opentelemetry::metrics::MeterProvider> api_provider{ ::std::move(sdk_provider) };\
    ::opentelemetry::metrics::Provider::SetMeterProvider(api_provider);\
}\
{\
    ::opentelemetry::exporter::otlp::OtlpGrpcExporterOptions options{};\
    options.endpoint = endpoint;\
    auto exporter  = ::opentelemetry::exporter::otlp::OtlpGrpcExporterFactory::Create(options);\
    auto processor = ::opentelemetry::sdk::trace::SimpleSpanProcessorFactory::Create(std::move(exporter));\
    auto resource = ::opentelemetry::sdk::resource::Resource::Create(\
        {\
            {"service.name", service_name}\
        });\
    auto sdk_provider = ::opentelemetry::sdk::trace::TracerProviderFactory::Create(std::move(processor), resource);\
    ::std::shared_ptr<::opentelemetry::trace::TracerProvider> api_provider{ ::std::move(sdk_provider) };\
    ::opentelemetry::trace::Provider::SetTracerProvider(api_provider);\
}\
auto meter = ::opentelemetry::metrics::Provider::GetMeterProvider()\
    ->GetMeter(service_name);\
auto m_cpu_total = meter->CreateInt64ObservableGauge(\
    service_name\
    "_cpu_total"\
);\
m_cpu_total->AddCallback(on_cpu_total, nullptr);\
auto m_memory_total = meter->CreateInt64ObservableGauge(\
    service_name\
    "_memory_total"\
);\
m_memory_total->AddCallback(on_memory_total, nullptr);

#define DECLARE_LOGGER()\
::opentelemetry::nostd::shared_ptr<::opentelemetry::logs::Logger> _logger;
#define DELCARE_TRACER()\
::opentelemetry::nostd::shared_ptr<::opentelemetry::trace::Tracer> _tracer;
#define DECLARE_METER()\
::opentelemetry::nostd::shared_ptr<::opentelemetry::metrics::Meter> _meter;
#define DECLARE_GAUGE(identifier)\
::opentelemetry::nostd::unique_ptr<::opentelemetry::metrics::Gauge<int64_t>> identifier;
#define DECLARE_COUNTER(identifier)\
::opentelemetry::nostd::unique_ptr<::opentelemetry::metrics::Counter<uint64_t>> identifier;
#define DECLARE_ONLY_SPAN(identifier)\
::opentelemetry::nostd::shared_ptr<::opentelemetry::trace::Span> identifier
#define DECLARE_SPAN(identifier)\
, ::opentelemetry::nostd::shared_ptr<::opentelemetry::trace::Span> identifier

#define INIT_LOGGER(name)\
, _logger{\
    ::opentelemetry::logs::Provider::GetLoggerProvider()\
        ->GetLogger(name)\
}
#define INIT_TRACER(name)\
, _tracer{\
    ::opentelemetry::trace::Provider::GetTracerProvider()\
        ->GetTracer(name)\
}
#define INIT_METER(name)\
, _meter{\
    ::opentelemetry::metrics::Provider::GetMeterProvider()\
        ->GetMeter(name)\
}
#define INIT_GAUGE(identifier, name)\
, identifier{ _meter->CreateInt64Gauge(name) }
#define INIT_COUNTER(identifier, name)\
, identifier{ _meter->CreateUInt64Counter(name) }

#define START_SPAN(identifier, name, context_identifier)\
::opentelemetry::nostd::shared_ptr<::opentelemetry::trace::Span> identifier{\
    context_identifier._tracer->StartSpan(name)\
};
#define START_SUBSPAN(identifier, name, parent_identifier, context_identifier)\
::opentelemetry::nostd::shared_ptr<::opentelemetry::trace::Span> identifier{\
    context_identifier._tracer->StartSpan(name, ::opentelemetry::trace::StartSpanOptions{\
        {}, {},\
        parent_identifier->GetContext()\
    })\
};
#define RESTART_SUBSPAN(identifier, name, parent_identifier, context_identifier)\
identifier = context_identifier._tracer->StartSpan(name, ::opentelemetry::trace::StartSpanOptions{\
    {}, {},\
    parent_identifier->GetContext()\
});
#define PROPAGATE_ONLY_SPAN(identifier)\
identifier
#define PROPAGATE_SPAN(identifier)\
, identifier

#define STOP_SPAN(identifier)\
identifier = nullptr;
#define START_SUBSCOPE(identifier, name, parent_identifier, context_identifier)\
::opentelemetry::trace::Scope identifier{\
    context_identifier._tracer->StartSpan(name, ::opentelemetry::trace::StartSpanOptions\
    {\
        {}, {},\
        parent_identifier->GetContext()\
    })\
};

#define LOG_INFO(what_data, what_size, parent_identifier, context_identifier)\
context_identifier._logger->Info(::opentelemetry::nostd::string_view{ what_data, what_size },\
    parent_identifier->GetContext());
#define TAG(name, value)\
{name, value}
#define ADD_COUNTER(identifier, value, tags)\
identifier->Add(value, tags);
#define RECORD_GAUGE(identifier, what, tags)\
identifier->Record(static_cast<int64_t>(what), tags);

#else

#define SYSTEM_CALLBACKS()
#define INIT_TELEMETRY(endpoint, service_name)

#define DECLARE_LOGGER()
#define DELCARE_TRACER()
#define DECLARE_METER()
#define DECLARE_GAUGE(identifier)
#define DECLARE_COUNTER(identifier)
#define DECLARE_ONLY_SPAN(identifier)
#define DECLARE_SPAN(identifier)

#define INIT_LOGGER(name)
#define INIT_TRACER(name)
#define INIT_METER(name)
#define INIT_GAUGE(identifier, name)
#define INIT_COUNTER(identifier, name)

#define START_SPAN(identifier, name, context_identifier)
#define START_SUBSPAN(identifier, name, parent_identifier, context_identifier)
#define RESTART_SUBSPAN(identifier, name, parent_identifier, context_identifier)
#define PROPAGATE_ONLY_SPAN(identifier)
#define PROPAGATE_SPAN(identifier)
#define STOP_SPAN(identifier)
#define START_SUBSCOPE(identifier, name, parent_identifier, context_identifier)

#define LOG_INFO(what_data, what_size, parent_identifier, context_identifier)
#define TAG(name, value)
#define ADD_COUNTER(identifier, value, tags)
#define RECORD_GAUGE(identifier, what, tags)

#endif

#endif
