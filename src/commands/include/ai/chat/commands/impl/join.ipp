#ifndef AI_CHAT_COMMANDS_JOIN_IPP
#define AI_CHAT_COMMANDS_JOIN_IPP

namespace ai {
namespace chat {
namespace commands {

template<typename Client>
join<Client>::join(Client &client)
    : _client{ client } {

};

template<typename Client>
::std::string_view join<Client>::execute(::std::string_view args) {
    _client.join(args);
    return args;
};

} // commands
} // chat
} // ai

#endif
