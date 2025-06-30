#ifndef AI_CHAT_CLIENTS_TWITCH_HANDLERS_OBSERVABLE_IPP
#define AI_CHAT_CLIENTS_TWITCH_HANDLERS_OBSERVABLE_IPP

#include <functional>
#include <utility>

#include "ai/chat/clients/twitch/handlers/observable.hpp"

namespace ai {
namespace chat {
namespace clients {
namespace twitch {
namespace handlers {

class observable::subscription {
    friend irc<observable>;
    friend slot;

private:
    ::std::function<void(const message&)> on_message;
};

observable::slot::slot(slot&& other) {
    _p_target = other._p_target;
    _p_observer = other._p_observer;
    other._p_target = nullptr;
    other._p_observer = nullptr;
};

observable::slot::~slot() {
    if (_p_target) {
        _p_target->_subscriptions.erase(_p_observer);
    }
};

observable::slot& observable::slot::operator=(slot&& other) {
    _p_target = other._p_target;
    _p_observer = other._p_observer;
    other._p_target = nullptr;
    other._p_observer = nullptr;
    return *this;
};

template<typename Action>
void observable::slot::on_message(Action&& callback) {
    _p_target->_subscriptions[_p_observer]
        .on_message = ::std::forward<Action>(callback);
};

observable::slot::slot(const ::std::type_info* p_observer, observable* p_target)
    : _p_observer{ p_observer }
    , _p_target{ p_target } {

};

template<typename Observer>
observable::slot_type observable::subscribe() {
    return { &typeid(Observer), this };
};

} // handlers

template<>
void irc<handlers::observable>::on_message(const message& message) const {
    for (const auto& p_observer_n_subscription : _subscriptions) {
        const subscription& subscription{ p_observer_n_subscription.second };
        subscription.on_message(message);
    }
};

} // twitch
} // clients
} // chat
} // ai

#endif
