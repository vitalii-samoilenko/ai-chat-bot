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
    friend slot;

    ::std::function<void(const message&)> on_message;
    ::std::function<void(const command&)> on_command;
};

template<template <typename> class Client>
observable<Client>::slot::slot(slot&& other) {
    _target = other._target;
    _observer = other._observer;
    other._target = nullptr;
    other._observer = nullptr;
};

template<template <typename> class Client>
observable<Client>::slot::~slot() {
    if (_target) {
        _target->_subscriptions.erase(_observer);
    }
};

template<template <typename> class Client>
typename observable<Client>::slot& observable<Client>::slot::operator=(slot&& other) {
    _target = other._target;
    _observer = other._observer;
    other._target = nullptr;
    other._observer = nullptr;
    return *this;
};

template<template <typename> class Client>
template<typename Action>
void observable<Client>::slot::on_message(Action&& callback) {
    _target->_subscriptions[_observer]
        .on_message = ::std::forward<Action>(callback);
};
template<template <typename> class Client>
template<typename Action>
void observable<Client>::slot::on_command(Action&& callback) {
    _target->_subscriptions[_observer]
        .on_command = ::std::forward<Action>(callback);
};

template<template <typename> class Client>
observable<Client>::slot::slot(const ::std::type_info* observer, observable* target)
    : _observer{ observer }
    , _target{ target } {

};

template<template <typename> class Client>
template<typename... Args>
observable<Client>::observable(Args&& ...args)
    : Client<observable>( ::std::forward<Args>(args)... )
    , _subscriptions{} {

};

template<template <typename> class Client>
template<typename Observer>
typename observable<Client>::slot observable<Client>::subscribe() {
    return { &typeid(Observer), this };
};

template<template <typename> class Client>
void observable<Client>::on_message(const message& message) const {
    for (const auto& observer_n_subscription : _subscriptions) {
        const subscription& subscription{ observer_n_subscription.second };
        subscription.on_message(message);
    }
};
template<template <typename> class Client>
void observable<Client>::on_command(const command& command) const {
    for (const auto& observer_n_subscription : _subscriptions) {
        const subscription& subscription{ observer_n_subscription.second };
        subscription.on_command(command);
    }
};

} // clients
} // chat
} // ai

#endif
