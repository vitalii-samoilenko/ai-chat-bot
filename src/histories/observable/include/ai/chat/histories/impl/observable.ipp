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
    friend observable_iterator<History>;

    ::std::function<void(observable_iterator<History>)> on_message;
    ::std::function<void(observable_iterator<History>)> on_erase1;
    ::std::function<void(observable_iterator<History>, observable_iterator<History>)> on_erase2;
    ::std::function<void(observable_iterator<History>)> on_update;
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
template<typename Action,
    ::std::enable_if_t<
        ::std::is_invocable_v<Action, observable_iterator<History>>,
        bool>>
void slot<History>::on_erase(Action &&callback) {
    _target->_subscriptions[_observer]
        .on_erase1 = ::std::forward<Action>(callback);
};
template<typename History>
template<typename Action,
    ::std::enable_if_t<
        ::std::is_invocable_v<Action, observable_iterator<History>, observable_iterator<History>>,
        bool>>
void slot<History>::on_erase(Action &&callback) {
    _target->_subscriptions[_observer]
        .on_erase2 = ::std::forward<Action>(callback);
};
template<typename History>
template<typename Action>
void slot<History>::on_update(Action &&callback) {
    _target->_subscriptions[_observer]
        .on_update = ::std::forward<Action>(callback);
};

template<typename History>
slot<History>::slot(::std::type_info const *observer, observable<History> *target)
    : _observer{ observer }
    , _target{ target } {

};

template<typename History>
bool operator<(observable_iterator<History> const &lhs, ::std::chrono::nanoseconds rhs) {
    return operator<(static_cast<iterator const>(lhs), rhs);
};
template<typename History>
bool operator<(::std::chrono::nanoseconds lhs, observable_iterator<History> const &rhs) {
    return operator<(lhs, static_cast<iterator const>(rhs));
};

template<typename History>
observable_iterator<History>::observable_iterator(observable_iterator const &other)
    : iterator{ static_cast<iterator const &>(other) }
    , _target{ other._target } {

};
template<typename History>
observable_iterator<History>::observable_iterator(observable_iterator &&other)
    : iterator{ static_cast<iterator &&>(other) }
    , _target{ other._target } {
    other._target = nullptr;
};

template<typename History>
message observable_iterator<History>::operator*() {
    return iterator::operator*();
};
template<typename History>
observable_iterator<History> & observable_iterator<History>::operator++() {
    iterator::operator++();
    return *this;
};
template<typename History>
bool observable_iterator<History>::operator==(observable_iterator const &rhs) const {
    return iterator::operator==(rhs);
};

template<typename History>
observable_iterator<History> observable_iterator<History>::operator+(ptrdiff_t rhs) const {
    return observable_iterator{ _target, iterator::operator+(rhs) };
};
template<typename History>
observable_iterator<History> observable_iterator<History>::operator+(::std::chrono::nanoseconds rhs) const {
    return observable_iterator{ _target, iterator::operator+(rhs) };
};
template<typename History>
ptrdiff_t observable_iterator<History>::operator-(observable_iterator rhs) const {
    return iterator::operator-(rhs);
};

template<typename History>
observable_iterator<History> & observable_iterator<History>::operator&=(tag rhs) {
    iterator::operator&=(rhs);
    return *this;
};

template<typename History>
template<typename Client>
observable_iterator<History> & observable_iterator<History>::operator=(::std::string_view rhs) {
    iterator::operator=(rhs);
    for (auto const &observer_n_subscription : _target->_subscriptions) {
        ::std::type_info const *observer{ observer_n_subscription.first };
        typename observable<History>::subscription const &subscription{ observer_n_subscription.second };
        if (observer == &typeid(Client)) {
            continue;
        }
        subscription.on_update(*this);
    }
    return *this;
};

template<typename History>
template<typename... Args>
observable_iterator<History>::observable_iterator(observable<History> *target,
    Args &&...args)
    : iterator{ ::std::forward<Args>(args)... }
    , _target{ target } {

};

template<typename History>
template<typename... Args>
observable<History>::observable(Args &&...args)
    : History{ ::std::forward<Args>(args)... }
    , _subscriptions{} {

};

template<typename History>
template<typename Observer>
slot<History> observable<History>::subscribe() {
    return slot<History>{ &typeid(Observer), this };
};

template<typename History>
observable_iterator<History> observable<History>::begin() {
    return observable_iterator<History>{ this, History::begin() };
};
template<typename History>
observable_iterator<History> observable<History>::end() {
    return observable_iterator<History>{ this, History::end() };
};

template<typename History>
observable_iterator<History> observable<History>::lower_bound(::std::chrono::nanoseconds timestamp) {
    return observable_iterator<History>{ this, History::lower_bound(timestamp) };
};

template<typename History>
template<typename Client>
observable_iterator<History> observable<History>::insert(message message) {
    observable_iterator<History> pos{ this, History::insert(message) };
    for (auto const &observer_n_subscription : _subscriptions) {
        ::std::type_info const *observer{ observer_n_subscription.first };
        subscription const &subscription{ observer_n_subscription.second };
        if (observer == &typeid(Client)) {
            continue;
        }
        subscription.on_message(pos);
    }
    return pos;
};
template<typename History>
template<typename Client>
observable_iterator<History> observable<History>::erase(observable_iterator<History> pos) {
    for (auto const &observer_n_subscription : _subscriptions) {
        ::std::type_info const *observer{ observer_n_subscription.first };
        subscription const &subscription{ observer_n_subscription.second };
        if (observer == &typeid(Client)) {
            continue;
        }
        subscription.on_erase1(pos);
    }
    return observable_iterator<History>{ this, History::erase(pos) };
};
template<typename History>
template<typename Client>
observable_iterator<History> observable<History>::erase(observable_iterator<History> first, observable_iterator<History> last) {
    for (auto const &observer_n_subscription : _subscriptions) {
        ::std::type_info const *observer{ observer_n_subscription.first };
        subscription const &subscription{ observer_n_subscription.second };
        if (observer == &typeid(Client)) {
            continue;
        }
        subscription.on_erase2(first, last);
    }
    return observable_iterator<History>{ this, History::erase(first, last) };
};

} // histories
} // chat
} // ai

#endif
