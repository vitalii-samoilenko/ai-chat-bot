#ifndef AI_CHAT_ADAPTERS_RATE_LIMITED_HPP
#define AI_CHAT_ADAPTERS_RATE_LIMITED_HPP

#include <chrono>
#include <utility>

namespace ai {
namespace chat {
namespace adapters {

template<typename Adapter>
class rate_limited : private Adapter {
public:
    rate_limited() = delete;
    rate_limited(const rate_limited&) = delete;
    rate_limited(rate_limited&&) = delete;

    ~rate_limited() = default;

    rate_limited& operator=(const rate_limited&) = delete;
    rate_limited& operator=(rate_limited&&) = delete;

    template<typename... Args>
    rate_limited(size_t rpm, Args&& ...args);

    ::std::pair<::std::string, size_t> complete() const;

private:
    ::std::chrono::nanoseconds _span;
    ::std::chrono::nanoseconds _next;
};

} // adapters
} // chat
} // ai

#include "impl/rate_limited.ipp"

#endif
