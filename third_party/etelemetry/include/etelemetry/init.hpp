#ifndef ETELEMETRY_INIT_HPP
#define ETELEMETRY_INIT_HPP

#ifndef ET_ENABLED

#define ET_INIT(endpoint, service_name)

#else

#include <utility>

#include "opentelemetry/exporters/otlp/otlp_grpc_log_record_exporter_factory.h"
#include "opentelemetry/sdk/logs/simple_log_record_processor_factory.h"
#include "opentelemetry/sdk/logs/logger_provider_factory.h"

#include "opentelemetry/exporters/otlp/otlp_grpc_metric_exporter_factory.h"
#include "opentelemetry/sdk/metrics/export/periodic_exporting_metric_reader_factory.h"
#include "opentelemetry/sdk/metrics/view/view_registry_factory.h"
#include "opentelemetry/sdk/metrics/meter_context_factory.h"
#include "opentelemetry/sdk/metrics/meter_provider_factory.h"

#include "opentelemetry/exporters/otlp/otlp_grpc_exporter_factory.h"
#include "opentelemetry/sdk/trace/simple_processor_factory.h"
#include "opentelemetry/sdk/trace/tracer_provider_factory.h"

#include "etelemetry/common.hpp"

#define ET_INIT(endpoint_address, service_name)\
{\
	::opentelemetry::exporter::otlp::OtlpGrpcLogRecordExporterOptions options{};\
	options.endpoint = endpoint_address;\
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
	options.endpoint = endpoint_address;\
	auto exporter = ::opentelemetry::exporter::otlp::OtlpGrpcMetricExporterFactory::Create(options);\
	::opentelemetry::sdk::metrics::PeriodicExportingMetricReaderOptions readerOptions{};\
	readerOptions.export_interval_millis = ::std::chrono::milliseconds{ 1000 };\
	readerOptions.export_timeout_millis = ::std::chrono::milliseconds{ 500 };\
	auto reader = ::opentelemetry::sdk::metrics::PeriodicExportingMetricReaderFactory::Create(::std::move(exporter), readerOptions);\
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
	options.endpoint = endpoint_address;\
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
auto _ET_PREPEND(_meter) = ::opentelemetry::metrics::Provider::GetMeterProvider()\
	->GetMeter(service_name);\
auto _ET_PREPEND(_cpu_total) = _ET_PREPEND(_meter)->CreateInt64ObservableGauge(\
	service_name\
	"_cpu_total"\
);\
_ET_PREPEND(_cpu_total)->AddCallback(_ET_PREPEND(_on_cpu_total), nullptr);\
auto _ET_PREPEND(_memory_total) = _ET_PREPEND(_meter)->CreateInt64ObservableGauge(\
	service_name\
	"_memory_total"\
);\
_ET_PREPEND(_memory_total)->AddCallback(_ET_PREPEND(_on_memory_total), nullptr)

#endif

#endif
