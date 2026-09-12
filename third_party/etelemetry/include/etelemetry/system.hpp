#ifndef ETELEMETRY_SYSTEM_HPP
#define ETELEMETRY_SYSTEM_HPP

#ifndef ET_ENABLED

#define ET_SYSTEM_CALLBACKS()

#else

#include "opentelemetry/metrics/observer_result.h"

#include "estd/system.hpp"

#include "etelemetry/common.hpp"

#define ET_SYSTEM_CALLBACKS()\
void _ET_PREPEND(_on_cpu_total)(::opentelemetry::metrics::ObserverResult observer_result, void *state) {\
	auto usage = ::estd::system::cpu::get_usage();\
	auto gauge = ::opentelemetry::nostd::get<\
		::opentelemetry::nostd::shared_ptr<\
		::opentelemetry::metrics::ObserverResultT<int64_t>\
		>>(observer_result);\
	gauge->Observe(usage.system, {\
		{"mode", "system"}\
	});\
	gauge->Observe(usage.user, {\
		{"mode", "user"}\
	});\
};\
\
void _ET_PREPEND(_on_memory_total)(::opentelemetry::metrics::ObserverResult observer_result, void *state) {\
	auto usage = ::estd::system::memory::get_usage();\
	auto gauge = ::opentelemetry::nostd::get<\
		::opentelemetry::nostd::shared_ptr<\
		::opentelemetry::metrics::ObserverResultT<int64_t>\
		>>(observer_result);\
	gauge->Observe(usage.anonymous, {\
		{"type", "anonymous"}\
	});\
	gauge->Observe(usage.shared, {\
		{"type", "shared"}\
	});\
}

#endif

#endif
