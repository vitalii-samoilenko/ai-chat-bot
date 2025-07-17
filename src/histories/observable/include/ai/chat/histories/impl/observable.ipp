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
    friend slot<History>;

    ::std::function<void(iterator)> on_message;
    ::std::function<void(iterator, iterator)> on_erase;
};

template<typename History>
slot<History>::slot(slot &&other) {
    _observer = other._observer;
    _target = other._target;
    other._observer = nullptr;
    other._target = nullptr;
};

template<typename History>
slot<History>::~slot() {
    if (_target) {
        _target->_subscriptions.erase(_observer);
    }
};

template<typename History>
template<typename Action>
void slot<History>::on_message(Action &&callback) {
    _target->_subscriptions[_observer]
        .on_message = ::std::forward<Action>(callback);
};
template<typename History>
template<typename Action>
void slot<History>::on_erase(Action &&callback) {
    _target->_subscriptions[_observer]
        .on_erase = ::std::forward<Action>(callback);
};

template<typename History>
slot<History>::slot(::std::type_info const *observer, observable<History> *target)
    : _observer{ observer }
    , _target{ target } {

};

template<typename History>
template<typename... Args>
observable<History>::observable(Args &&...args)
    : History{ ::std::forward<Args>(args)... }
    , _subscriptions{} {

}

template<typename History>
template<typename Observer>
slot<History> observable<History>::subscribe() {
    return slot<History>{ &typeid(Observer), this };
};

template<typename History>
template<typename Client>
iterator observable<History>::insert(message message) {
    iterator iterator{ History::insert(message) };
    for (auto const &observer_n_subscription : _subscriptions) {
        ::std::type_info const *observer{ observer_n_subscription.first };
        subscription const &subscription{ observer_n_subscription.second };
        if (observer == &typeid(Client)) {
            continue;
        }
        subscription.on_message(iterator);
    }
    return iterator;
};
template<typename History>
template<typename Client>
iterator observable<History>::erase(iterator first, iterator last) {
    for (auto const &observer_n_subscription : _subscriptions) {
        ::std::type_info const *observer{ observer_n_subscription.first };
        subscription const &subscription{ observer_n_subscription.second };
        if (observer == &typeid(Client)) {
            continue;
        }
        subscription.on_erase(first, last);
    }
    return History::erase(first, last);
};

} // histories
} // chat
} // ai

#endif
