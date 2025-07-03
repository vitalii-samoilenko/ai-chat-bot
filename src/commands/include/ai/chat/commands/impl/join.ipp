#ifndef AI_CHAT_COMMANDS_JOIN_IPP
#define AI_CHAT_COMMANDS_JOIN_IPP

#include "ai/chat/commands/join.hpp"

namespace ai {
namespace chat {
namespace commands {

template<typename Client>
join<Client>::join(Client& client)
    : _client{ client } {

};

template<typename Client>
::std::string join<Client>::execute(const ::std::string& args) {
    _client.join(args);
    return args;
};

} // commands
} // chat
} // ai

#endif
