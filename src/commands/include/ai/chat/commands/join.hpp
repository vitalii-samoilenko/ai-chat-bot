#ifndef AI_CHAT_COMMANDS_JOIN_HPP
#define AI_CHAT_COMMANDS_JOIN_HPP

#include <string_view>

#include "re2/re2.h"

#include "ai/chat/clients/observable.hpp"

namespace ai {
namespace chat {
namespace commands {

template<template <typename> class Client>
class join {
public:
    static char constexpr name[]{ "join" };

    join() = delete;
    join(join const &other) = delete;
    join(join &&other) = delete;

    ~join() = default;

    join & operator=(join const &other) = delete;
    join & operator=(join &&other) = delete;

    explicit join(::ai::chat::clients::observable<Client> &client);

    ::std::string_view execute(::std::string_view args);

private:
    ::ai::chat::clients::observable<Client> &_client;
    ::RE2 _parser;
};

} // commands
} // chat
} // ai

#include "impl/join.ipp"

#endif
