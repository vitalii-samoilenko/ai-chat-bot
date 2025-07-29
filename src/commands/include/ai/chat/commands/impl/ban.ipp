#ifndef AI_CHAT_COMMANDS_BAN_IPP
#define AI_CHAT_COMMANDS_BAN_IPP

#include "ai/chat/commands/ban.hpp"

namespace ai {
namespace chat {
namespace commands {

template<typename Moderator>
ban<Moderator>::ban(Moderator &moderator)
    : _moderator{ moderator }
    , _parser{ R"([a-z0-9_]+)" } {

};

template<typename Moderator>
::std::string_view ban<Moderator>::execute(::std::string_view args) {
    if (!::RE2::FullMatch(args, _parser)) {
        return ::std::string_view{};
    }
    _moderator.ban(args);
    return ban::name;
};

} // commands
} // chat
} // ai

#endif
