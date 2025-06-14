#ifndef AI_CHAT_ADAPTERS_CONTENT_LENGTH_IPP
#define AI_CHAT_ADAPTERS_CONTENT_LENGTH_IPP

#include "opentelemetry/metrics/meter_provider.h"

#include "ai/chat/adapters/content_length.hpp"

namespace ai {
namespace chat {
namespace adapters {

template<typename Adapter>
template<typename... Args>
content_length<Adapter>::content_length(Args&& ...args)
    : Adapter{ ::std::forward<Args>(args)...  }
    , _p_content_length{
        ::opentelemetry::metrics::Provider::GetMeterProvider()
            ->GetMeter(
                "ai_chat_adapters_telemetry_meter"
            )
            ->CreateUInt64Histogram(
                "ai_chat_adapters_telemetry_content_length"
            )
    } {

};

template<typename Adapter>
::std::pair<::std::string, size_t> content_length<Adapter>::complete() const {
    ::std::pair<::std::string, size_t> result{ Adapter::complete() };
    _p_content_length->Record(static_cast<uint64_t>(result.first.size()));
    return result;
};

} // adapters
} // chat
} // ai

#endif
