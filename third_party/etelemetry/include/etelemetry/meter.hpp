#ifndef ETELEMETRY_METER_HPP
#define ETELEMETRY_METER_HPP

#ifndef ET_ENABLED

#define ET_DECLARE_METER()
#define ET_DECLARE_COUNTER(identifier)
#define ET_DECLARE_GAUGE(identifier)
#define a_ET_INIT_METER(name)
#define a_ET_INIT_COUNTER(identifier, name)
#define a_ET_INIT_GAUGE(identifier, name)

#define ET_TAG(name, value)
#define ET_ADD_COUNTER(context_identifier, identifier, value, tags)
#define ET_RECORD_GAUGE(context_identifier, identifier, what, tags)

#else

#include "opentelemetry/metrics/provider.h"

#include "etelemetry/common.hpp"

#define ET_DECLARE_METER()\
::opentelemetry::nostd::shared_ptr<::opentelemetry::metrics::Meter> _ET_PREPEND(_meter)

#define ET_DECLARE_COUNTER(identifier)\
::opentelemetry::nostd::unique_ptr<::opentelemetry::metrics::Counter<uint64_t>> _ET_PREPEND(identifier)

#define ET_DECLARE_GAUGE(identifier)\
::opentelemetry::nostd::unique_ptr<::opentelemetry::metrics::Gauge<int64_t>> _ET_PREPEND(identifier)

#define a_ET_INIT_METER(name)\
, _ET_PREPEND(_meter){\
	::opentelemetry::metrics::Provider::GetMeterProvider()\
		->GetMeter(name)\
}

#define a_ET_INIT_COUNTER(identifier, name)\
, _ET_PREPEND(identifier){ _ET_PREPEND(_meter)->CreateUInt64Counter(name) }

#define a_ET_INIT_GAUGE(identifier, name)\
, _ET_PREPEND(identifier){ _ET_PREPEND(_meter)->CreateInt64Gauge(name) }

#define ET_TAG(name, value)\
{name, value}

#define ET_ADD_COUNTER(context_identifier, identifier, value, tags)\
context_identifier._ET_PREPEND(identifier)->Add(value, tags)

#define ET_RECORD_GAUGE(context_identifier, identifier, what, tags)\
context_identifier._ET_PREPEND(identifier)->Record(static_cast<int64_t>(what), tags)

#endif

#endif
