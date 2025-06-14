#ifndef AI_CHAT_ADAPTERS_TRACE_IPP
#define AI_CHAT_ADAPTERS_TRACE_IPP

#include "opentelemetry/trace/tracer_provider.h"

#include "ai/chat/adapters/trace.hpp"

namespace ai {
namespace chat {
namespace adapters {

template<typename Adapter>
template<typename... Args>
trace<Adapter>::trace(Args&& ...args)
    : _next{ ::std::forward<Args>(args)...  }
    , _p_tracer{
        ::opentelemetry::trace::Provider::GetTracerProvider()
            ->GetTracer(
                "ai_chat_adapters_telemetry_tracer"
            )
    } {

};

template<typename Adapter>
Adapter& trace<Adapter>::next() {
    return _next;
};

template<typename Adapter>
::std::pair<::std::string, size_t> trace<Adapter>::complete() const {
    auto scope = _p_tracer->WithActiveSpan(
        _p_tracer->StartSpan("adapter::complete"));
    return _next.complete();
};

} // adapters
} // chat
} // ai

#endif
