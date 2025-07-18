#ifndef AI_CHAT_COMMANDS_BAN_HPP
#define AI_CHAT_COMMANDS_BAN_HPP

#include "re2/re2.h"

#include <string_view>

namespace ai {
namespace chat {
namespace commands {

template<typename Moderator>
class ban {
public:
    static char constexpr name[]{ "ban" };

    ban() = delete;
    ban(ban const &other) = delete;
    ban(ban &&other) = delete;

    ~ban() = default;

    ban & operator=(ban const &other) = delete;
    ban & operator=(ban &&other) = delete;

    explicit ban(Moderator &moderator);

    ::std::string_view execute(::std::string_view args);

private:
    Moderator &_moderator;
    ::RE2 _parser;
};

} // commands
} // chat
} // ai

#include "impl/ban.ipp"

#endif
