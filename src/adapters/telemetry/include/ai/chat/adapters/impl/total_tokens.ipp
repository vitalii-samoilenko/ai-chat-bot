#ifndef AI_CHAT_ADAPTERS_TOTAL_TOKENS_IPP
#define AI_CHAT_ADAPTERS_TOTAL_TOKENS_IPP

#include "opentelemetry/metrics/meter_provider.h"

#include "ai/chat/adapters/total_tokens.hpp"

namespace ai {
namespace chat {
namespace adapters {

template<typename Adapter>
template<typename... Args>
total_tokens<Adapter>::total_tokens(Args&& ...args)
    : _next{ ::std::forward<Args>(args)...  }
    , _p_total_tokens{
        ::opentelemetry::metrics::Provider::GetMeterProvider()
            ->GetMeter(
                "ai_chat_adapters_telemetry_meter"
            )
            ->CreateInt64Gauge(
                "ai_chat_adapters_telemetry_total_tokens"
            )
    } {

};

template<typename Adapter>
Adapter& total_tokens<Adapter>::next() {
    return _next;
};

template<typename Adapter>
::std::pair<::std::string, size_t> total_tokens<Adapter>::complete() const {
    ::std::pair<::std::string, size_t> result{ _next.complete() };
    _p_total_tokens->Record(static_cast<int64_t>(result.second));
    return result;
};

} // adapters
} // chat
} // ai

#endif
