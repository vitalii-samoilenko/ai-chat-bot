#ifndef AI_CHAT_COMMANDS_DENY_HPP
#define AI_CHAT_COMMANDS_DENY_HPP

#include <string_view>

#include "re2/re2.h"

namespace ai {
namespace chat {
namespace commands {

template<typename Moderator>
class deny {
public:
    static char constexpr name[]{ "deny" };

    deny() = delete;
    deny(deny const &other) = delete;
    deny(deny &&other) = delete;

    ~deny() = default;

    deny & operator=(deny const &other) = delete;
    deny & operator=(deny &&other) = delete;

    explicit deny(Moderator &moderator);

    ::std::string_view execute(::std::string_view args);

private:
    Moderator &_moderator;
    ::RE2 _parser;
};

} // commands
} // chat
} // ai

#include "impl/deny.ipp"

#endif
