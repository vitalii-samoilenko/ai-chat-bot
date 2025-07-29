#ifndef AI_CHAT_COMMANDS_TIMEOUT_IPP
#define AI_CHAT_COMMANDS_TIMEOUT_IPP

#include <chrono>

#include "ai/chat/commands/timeout.hpp"

namespace ai {
namespace chat {
namespace commands {

template<typename Moderator>
timeout<Moderator>::timeout(Moderator &moderator)
    : _moderator{ moderator }
    , _parser{ R"((?<username>[a-z0-9_]+) (?<hours>[0-9]{2}):(?<minutes>[0-9]{2}):(?<seconds>[0-9]{2}))" } {

};

template<typename Moderator>
::std::string_view timeout<Moderator>::execute(::std::string_view args) {
    ::std::string_view username{};
    int hours{}, minutes{}, seconds{};
    if (!::RE2::FullMatch(args, _parser,
            &username, &hours, &minutes, &seconds)) {
        return ::std::string_view{};
    }
    auto until = (::std::chrono::utc_clock::now()
            + ::std::chrono::hours{ hours }
            + ::std::chrono::minutes{ minutes }
            + ::std::chrono::seconds{ seconds })
        .time_since_epoch();
    _moderator.timeout(username,
        ::std::chrono::duration_cast<::std::chrono::seconds>(until));
    return timeout::name;
};

} // commands
} // chat
} // ai

#endif
