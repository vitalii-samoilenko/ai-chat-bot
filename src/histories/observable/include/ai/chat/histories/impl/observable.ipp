#ifndef AI_CHAT_HISTORIES_OBSERVABLE_IPP
#define AI_CHAT_HISTORIES_OBSERVABLE_IPP

#include <functional>
#include <utility>

namespace ai {
namespace chat {
namespace histories {

template<typename History>
class observable<History>::subscription {
private:
    friend observable;
    friend slot;

    ::std::function<void(const message&)> on_message;
};

template<typename History>
observable<History>::slot::slot(slot&& other) {
    _observer = other._observer;
    _target = other._target;
    other._observer = nullptr;
    other._target = nullptr;
};

template<typename History>
observable<History>::slot::~slot() {
    if (_target) {
        _target->_subscriptions.erase(_observer);
    }
};

template<typename History>
typename observable<History>::slot& observable<History>::slot::operator=(slot&& other) {
    _observer = other._observer;
    _target = other._target;
    other._observer = nullptr;
    other._target = nullptr;
    return *this;
};

template<typename History>
observable<History>::slot::slot(const ::std::type_info* observer, observable* target)
    : _observer{ observer }
    , _target{ target } {

};

template<typename History>
template<typename Action>
void observable<History>::slot::on_message(Action&& callback) {
    _target->_subscriptions[_observer]
        .on_message = ::std::forward<Action>(callback);
};

template<typename History>
template<typename... Args>
observable<History>::observable(Args&& ...args)
    : History{ ::std::forward<Args>(args)... }
    , _subscriptions{} {

}

template<typename History>
template<typename Observer>
typename observable<History>::slot observable<History>::subscribe() {
    return { &typeid(Observer), this };
};

template<typename History>
template<typename Client>
typename observable<History>::iterator observable<History>::insert(const message& message) {
    iterator iterator{ History::insert(message) };
    for (const auto& observer_n_subscription : _subscriptions) {
        const ::std::type_info* observer{ observer_n_subscription.first };
        const subscription& subscription{ observer_n_subscription.second };
        if (observer == &typeid(Client)) {
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
