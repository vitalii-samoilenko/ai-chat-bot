#ifndef AI_CHAT_COMMANDS_LEAVE_IPP
#define AI_CHAT_COMMANDS_LEAVE_IPP

namespace ai {
namespace chat {
namespace commands {

template<template <typename> class Client>
leave<Client>::leave(::ai::chat::clients::observable<Client> &client)
    : _client{ client } {

};

template<template <typename> class Client>
::std::string_view leave<Client>::execute(::std::string_view args) {
    if (!args.empty()) {
        return ::std::string_view{};
    }
    _client.leave();
    return leave::name;
};

} // commands
} // chat
} // ai

#endif
