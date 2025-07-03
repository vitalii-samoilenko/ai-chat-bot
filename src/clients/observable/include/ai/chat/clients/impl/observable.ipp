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
    friend observable;
    friend slot;

private:
    ::std::function<void(const message&)> on_message;
    ::std::function<void(const command&)> on_command;
};

template<template <typename> class Client>
observable<Client>::slot::slot(slot&& other) {
    _p_target = other._p_target;
    _p_observer = other._p_observer;
    other._p_target = nullptr;
    other._p_observer = nullptr;
};

template<template <typename> class Client>
observable<Client>::slot::~slot() {
    if (_p_target) {
        _p_target->_subscriptions.erase(_p_observer);
    }
};

template<template <typename> class Client>
typename observable<Client>::slot& observable<Client>::slot::operator=(slot&& other) {
    _p_target = other._p_target;
    _p_observer = other._p_observer;
    other._p_target = nullptr;
    other._p_observer = nullptr;
    return *this;
};

template<template <typename> class Client>
template<typename Action>
void observable<Client>::slot::on_message(Action&& callback) {
    _p_target->_subscriptions[_p_observer]
        .on_message = ::std::forward<Action>(callback);
};
template<template <typename> class Client>
template<typename Action>
void observable<Client>::slot::on_command(Action&& callback) {
    _p_target->_subscriptions[_p_observer]
        .on_command = ::std::forward<Action>(callback);
};

template<template <typename> class Client>
observable<Client>::slot::slot(const ::std::type_info* p_observer, observable* p_target)
    : _p_observer{ p_observer }
    , _p_target{ p_target } {

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
    for (const auto& p_observer_n_subscription : _subscriptions) {
        const subscription& subscription{ p_observer_n_subscription.second };
        subscription.on_message(message);
    }
};
template<template <typename> class Client>
void observable<Client>::on_command(const command& command) const {
    for (const auto& p_observer_n_subscription : _subscriptions) {
        const subscription& subscription{ p_observer_n_subscription.second };
        subscription.on_command(command);
    }
};

} // clients
} // chat
} // ai

#endif
