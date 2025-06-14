#ifndef AI_CHAT_ADAPTERS_LOG_IPP
#define AI_CHAT_ADAPTERS_LOG_IPP

#include "opentelemetry/logs/logger_provider.h"

#include "ai/chat/adapters/log.hpp"

namespace ai {
namespace chat {
namespace adapters {

template<typename Adapter>
template<typename... Args>
log<Adapter>::log(Args&& ...args)
    : Adapter{ ::std::forward<Args>(args)...  }
    , _p_logger{
        ::opentelemetry::logs::Provider::GetLoggerProvider()
            ->GetLogger(
                "ai_chat_adapters_telemetry_logger"
            )
    } {

};

template<typename Adapter>
::std::pair<::std::string, size_t> log<Adapter>::complete() const {
    ::std::pair<::std::string, size_t> result{ Adapter::complete() };
    _p_logger->Info(result.first);
    return result;
};

} // adapters
} // chat
} // ai

#endif
