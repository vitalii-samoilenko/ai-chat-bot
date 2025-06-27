#ifndef AI_CHAT_HISTORIES_OBSERVABLE_IPP
#define AI_CHAT_HISTORIES_OBSERVABLE_IPP

#include <functional>
#include <utility>

#include "ai/chat/histories/observable.hpp"

namespace ai {
namespace chat {
namespace histories {

template<typename History>
class observable<History>::subscription {
    friend observable;
    friend slot;

public:
    ::std::function<void(const message_type&)> on_message;
};

template<typename History>
observable<History>::slot::slot(slot&& other) {
    _p_observer = other._p_observer;
    _p_target = other._p_target
    other._p_observer = nullptr;
    other._p_target = nullptr;
};

template<typename History>
observable<History>::slot::~slot() {
    if (_p_target) {
        _p_target->_subscriptions.erase(_p_observer);
    }
};

template<typename History>
observable<History>::slot& observable<History>::slot::operator=(slot&& other) {
    _p_observer = other._p_observer;
    _p_target = other._p_target
    other._p_observer = nullptr;
    other._p_target = nullptr;
};

template<typename History>
observable<History>::slot::slot(const ::std::type_info* p_observer, observable* p_target)
    : _p_observer{ p_observer }
    , _p_target{ p_target } {

};

template<typename History>
template<typename Action>
void observable<History>::slot::on_message(Action&& callback) {
    _p_target->_subscriptions[_p_observer]
        .on_message = ::std::forward(callback);
};

template<typename History>
template<typename Observer>
observable<History>::slot_type observable<History>::subscribe() {
    return { &typeid(Observer), this };
};

template<typename History>
template<typename Client>
observable<History>::iterator_type observable<History>::insert(const message_type& message) {
    iterator_type iterator{ History::insert(message) };
    for (const auto& p_observer_n_subscription : _subscriptions) {
        const ::std::type_info* p_observer{ p_observer_n_subscription.first };
        const subscription& subscription{ p_observer_n_subscription.second };
        if (p_observer == &typeid(Client)) {
            continue;
        }
        subscription.on_message(message);
    }
    return iterator;
};

} // histories
} // chat
} // ai

#endif
