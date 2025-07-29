#ifndef AI_CHAT_COMMANDS_DENY_IPP
#define AI_CHAT_COMMANDS_DENY_IPP

#include "ai/chat/commands/deny.hpp"

namespace ai {
namespace chat {
namespace commands {

template<typename Moderator>
deny<Moderator>::deny(Moderator &moderator)
    : _moderator{ moderator }
    , _parser{ R"([a-z0-9_]+)" } {

};

template<typename Moderator>
::std::string_view deny<Moderator>::execute(::std::string_view args) {
    if (!::RE2::FullMatch(args, _parser)) {
        return ::std::string_view{};
    }
    _moderator.deny(args);
    return deny::name;
};

} // commands
} // chat
} // ai

#endif
