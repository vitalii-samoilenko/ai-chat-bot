#ifndef AI_CHAT_COMMANDS_LEAVE_HPP
#define AI_CHAT_COMMANDS_LEAVE_HPP

#include <string_view>

#include "ai/chat/clients/observable.hpp"

namespace ai {
namespace chat {
namespace commands {

template<template <typename> class Client>
class leave {
public:
    static char constexpr name[]{ "leave" };

    leave() = delete;
    leave(leave const &other) = delete;
    leave(leave &&other) = delete;

    ~leave() = default;

    leave & operator=(leave const &other) = delete;
    leave & operator=(leave &&other) = delete;

    explicit leave(::ai::chat::clients::observable<Client> &client);

    ::std::string_view execute(::std::string_view args);

private:
    ::ai::chat::clients::observable<Client> &_client;
};

} // commands
} // chat
} // ai

#include "impl/leave.ipp"

#endif
