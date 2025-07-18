#ifndef AI_CHAT_COMMANDS_UNBAN_HPP
#define AI_CHAT_COMMANDS_UNBAN_HPP

#include "re2/re2.h"

#include <string_view>

namespace ai {
namespace chat {
namespace commands {

template<typename Moderator>
class unban {
public:
    static char constexpr name[]{ "unban" };

    unban() = delete;
    unban(unban const &other) = delete;
    unban(unban &&other) = delete;

    ~unban() = default;

    unban & operator=(unban const &other) = delete;
    unban & operator=(unban &&other) = delete;

    explicit unban(Moderator &moderator);

    ::std::string_view execute(::std::string_view args);

private:
    Moderator &_moderator;
    ::RE2 _parser;
};

} // commands
} // chat
} // ai

#include "impl/unban.ipp"

#endif
