#ifndef AI_CHAT_ADAPTERS_RATE_LIMITED_IPP
#define AI_CHAT_ADAPTERS_RATE_LIMITED_IPP

#include <thread>

#include "ai/chat/adapters/rate_limited.hpp"

namespace ai {
namespace chat {
namespace adapters {

template<typename Adapter>
template<typename... Args>
rate_limited<Adapter>::rate_limited(size_t rpm, Args&& ...args)
    : Adapter{ ::std::forward<Args>(args)... }
    , _span{ 60 * 1000000000 / rpm  }
    , _next{ 0 } {

};

template<typename Adapter>
::std::pair<::std::string, size_t> rate_limited<Adapter>::complete() const {
    ::std::chrono::nanoseconds before{ ::std::chrono::steady_clock::now().time_since_epoch() };
    if (before < _next) {
        ::std::this_thread::sleep_for(_next - before);
    }
    ::std::pair<::std::string, size_t> result{ Adapter::complete() };
    ::std::chrono::minutes after{ ::std::chrono::steady_clock::now().time_since_epoch() };
    if (::std::chrono::duration_cast<::std::chrono::minutes>(_next) < after) {
        _next = after;
    }
    _next += _span;
};

} // adapters
} // chat
} // ai

#endif
