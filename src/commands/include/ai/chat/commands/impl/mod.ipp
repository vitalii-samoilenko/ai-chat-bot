#ifndef AI_CHAT_COMMANDS_MOD_IPP
#define AI_CHAT_COMMANDS_MOD_IPP

#include "ai/chat/commands/mod.hpp"

namespace ai {
namespace chat {
namespace commands {

template<typename Moderator>
mod<Moderator>::mod(Moderator &moderator)
    : _moderator{ moderator }
    , _parser{ R"([a-z0-9_]+)" } {

};

template<typename Moderator>
::std::string_view mod<Moderator>::execute(::std::string_view args) {
    if (!::RE2::FullMatch(args, _parser)) {
        return ::std::string_view{};
    }
    _moderator.mod(args);
    return mod::name;
};

} // commands
} // chat
} // ai

#endif
