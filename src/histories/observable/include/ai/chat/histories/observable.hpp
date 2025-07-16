#ifndef AI_CHAT_HISTORIES_OBSERVABLE_HPP
#define AI_CHAT_HISTORIES_OBSERVABLE_HPP

#include <typeinfo>
#include <unordered_map>

namespace ai {
namespace chat {
namespace histories {

template<typename History>
class observable;

template<typename History>
class slot {
public:
    slot() = delete;
    slot(slot const &other) = delete;
    slot(slot &&other);

    ~slot();

    slot & operator=(slot const &other) = delete;
    slot & operator=(slot &&other) = delete;

    template<typename Action>
    void on_message(Action &&callback);
    template<typename Action>
    void on_erase(Action &&callback);

private:
    friend observable<History>;

    slot(::std::type_info const *observer, observable<History> *target);

    ::std::type_info const *_observer;
    observable<History> *_target;
};

template<typename History>
class observable : private History {
public:
    observable() = delete;
    observable(observable const &other) = delete;
    observable(observable &&other) = delete;

    ~observable() = default;

    observable & operator=(observable const &other) = delete;
    observable & operator=(observable &&other) = delete;

    template<typename... Args>
    explicit observable(Args &&...args);

    template<typename Observer>
    slot<History> subscribe();

    using History::begin;
    using History::end;

    template<typename Client>
    iterator insert(message message);
    template<typename Client>
    iterator erase(iterator first, iterator last);

private:
    friend slot<History>;

    class subscription;

    ::std::unordered_map<::std::type_info const *, subscription> _subscriptions;
};

} // histories
} // chat
} // ai

#include "impl/observable.ipp"

#endif
