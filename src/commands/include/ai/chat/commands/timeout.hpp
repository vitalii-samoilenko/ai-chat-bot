#ifndef AI_CHAT_COMMANDS_TIMEOUT_HPP
#define AI_CHAT_COMMANDS_TIMEOUT_HPP

#include <string_view>

#include "re2/re2.h"

namespace ai {
namespace chat {
namespace commands {

template<typename Moderator>
class timeout {
public:
    static char constexpr name[]{ "timeout" };

    timeout() = delete;
    timeout(timeout const &other) = delete;
    timeout(timeout &&other) = delete;

    ~timeout() = default;

    timeout & operator=(timeout const &other) = delete;
    timeout & operator=(timeout &&other) = delete;

    explicit timeout(Moderator &moderator);

    ::std::string_view execute(::std::string_view args);

private:
    Moderator &_moderator;
    ::RE2 _parser;
};

} // commands
} // chat
} // ai

#include "impl/timeout.ipp"

#endif
