#ifndef AI_CHAT_CLIENTS_OBSERVABLE_IPP
#define AI_CHAT_CLIENTS_OBSERVABLE_IPP

#include <functional>
#include <utility>

#include "ai/chat/clients/observable.hpp"

namespace ai {
namespace chat {
namespace clients {

template<template <typename> class Client>
class observable<Client>::subscription {
private:
    friend observable;
    friend slot<Client>;

    ::std::function<void(message)> on_message{
        [](message)->void {}
    };
    ::std::function<void(command)> on_command{
        [](command)->void {}
    };
};

template<template <typename> class Client>
slot<Client>::slot(slot &&other) {
    _observer = other._observer;
    _target = other._target;
    other._observer = nullptr;
    other._target = nullptr;
};

template<template <typename> class Client>
slot<Client>::~slot() {
    if (_target) {
        _target->_subscriptions.erase(_observer);
    }
};

template<template <typename> class Client>
template<typename Action>
void slot<Client>::on_message(Action &&callback) {
    _target->_subscriptions[_observer]
        .on_message = ::std::forward<Action>(callback);
};
template<template <typename> class Client>
template<typename Action>
void slot<Client>::on_command(Action &&callback) {
    _target->_subscriptions[_observer]
        .on_command = ::std::forward<Action>(callback);
};

template<template <typename> class Client>
slot<Client>::slot(::std::type_info const *observer, observable<Client> *target)
    : _observer{ observer }
    , _target{ target } {

};

template<template <typename> class Client>
template<typename... Args>
observable<Client>::observable(Args &&...args)
    : Client<observable>( ::std::forward<Args>(args)... )
    , _subscriptions{} {

};

template<template <typename> class Client>
template<typename Observer>
slot<Client> observable<Client>::subscribe() {
    return slot<Client>{ &typeid(Observer), this };
};

template<template <typename> class Client>
void observable<Client>::on_message(message const &message) const {
    for (auto const &observer_n_subscription : _subscriptions) {
        subscription const &subscription{ observer_n_subscription.second };
        subscription.on_message(message);
    }
};
template<template <typename> class Client>
void observable<Client>::on_command(command const &command) const {
    for (auto const &observer_n_subscription : _subscriptions) {
        subscription const &subscription{ observer_n_subscription.second };
        subscription.on_command(command);
    }
};

} // clients
} // chat
} // ai

#endif
