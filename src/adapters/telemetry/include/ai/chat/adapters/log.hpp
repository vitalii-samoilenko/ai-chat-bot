#ifndef AI_CHAT_ADAPTERS_LOG_HPP
#define AI_CHAT_ADAPTERS_LOG_HPP

#include <string>
#include <utility>

#include "opentelemetry/logs/logger.h"
#include "opentelemetry/nostd/shared_ptr.h"

namespace ai {
namespace chat {
namespace adapters {

template<typename Adapter>
class log : private Adapter {
public:
    log() = delete;
    log(const log&) = delete;
    log(log&&) = delete;

    ~log() = default;

    log& operator=(const log&) = delete;
    log& operator=(log&&) = delete;

    template<typename... Args>
    explicit log(Args&& ...args);

    ::std::pair<::std::string, size_t> complete() const;

private:
    ::opentelemetry::nostd::shared_ptr<::opentelemetry::logs::Logger> _p_logger;
};

} // adapters
} // chat
} // ai

#include "impl/log.ipp"

#endif
