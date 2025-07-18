#ifndef AI_CHAT_COMMANDS_LEAVE_IPP
#define AI_CHAT_COMMANDS_LEAVE_IPP

namespace ai {
namespace chat {
namespace commands {

template<typename Client>
leave<Client>::leave(Client &client)
    : _client{ client } {

};

template<typename Client>
::std::string_view leave<Client>::execute(::std::string_view args) {
    if (!args.empty()) {
        return ::std::string_view{};
    }
    _client.leave();
    return ::std::string_view{ "ack" };
};

} // commands
} // chat
} // ai

#endif
