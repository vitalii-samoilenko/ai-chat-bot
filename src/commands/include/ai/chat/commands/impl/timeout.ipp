#ifndef AI_CHAT_COMMANDS_TIMEOUT_IPP
#define AI_CHAT_COMMANDS_TIMEOUT_IPP

#include <chrono>

namespace ai {
namespace chat {
namespace commands {

template<typename Moderator>
timeout<Moderator>::timeout(Moderator &moderator)
    : _moderator{ moderator }
    , _pattern{ R"((?<username>[a-z0-9_]+) (?<hours>[0-9]{2}):(?<minutes>[0-9]{2}):(?<seconds>[0-9]{2}))" } {

};

template<typename Moderator>
::std::string_view timeout<Moderator>::execute(::std::string_view args) {
    ::std::string_view username{};
    int hours{};
    int minutes{};
    int seconds{};
    if (!::RE2::FullMatch(args, _parser,
            &username, &hours, &minutes, &seconds)) {
        return ::std::string_view{};
    }
    auto until = ::std::chrono::utc_clock::now();
    until += ::std::chrono::hours{ hours };
    until += ::std::chrono::minutes{ minutes };
    until += ::std::chrono::seconds{ seconds };
    moderator.timeout(username, until);
    return args;
};

} // commands
} // chat
} // ai

#endif
