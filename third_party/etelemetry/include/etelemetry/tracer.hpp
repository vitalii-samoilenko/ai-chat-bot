#ifndef ETELEMETRY_TRACER_HPP
#define ETELEMETRY_TRACER_HPP

#ifndef ET_ENABLED

#define ET_DECLARE_TRACER()
#define a_ET_INIT_TRACER(name)

#define ET_START_SPAN(context_identifier, identifier, name)
#define ET_START_SUBSPAN(context_identifier, parent_identifier, identifier, name)
#define ET_STOP_SPAN(identifier)

#else

#include "opentelemetry/trace/provider.h"

#include "etelemetry/common.hpp"

#define ET_DECLARE_TRACER()\
::opentelemetry::nostd::shared_ptr<::opentelemetry::trace::Tracer> _ET_PREPEND(_tracer)

#define a_ET_INIT_TRACER(name)\
, _ET_PREPEND(_tracer){\
	::opentelemetry::trace::Provider::GetTracerProvider()\
		->GetTracer(name)\
}

#define ET_START_SPAN(context_identifier, identifier, name)\
::opentelemetry::nostd::shared_ptr<::opentelemetry::trace::Span> _ET_PREPEND(identifier){\
	context_identifier._ET_PREPEND(_tracer)->StartSpan(name)\
}

#define ET_START_SUBSPAN(context_identifier, parent_identifier, identifier, name)\
::opentelemetry::nostd::shared_ptr<::opentelemetry::trace::Span> _ET_PREPEND(identifier){\
	context_identifier._ET_PREPEND(_tracer)->StartSpan(name, ::opentelemetry::trace::StartSpanOptions{\
		{}, {},\
		_ET_PREPEND(parent_identifier)->GetContext()\
	})\
}

#define ET_STOP_SPAN(identifier)\
_ET_PREPEND(identifier) = nullptr


#endif

#endif
