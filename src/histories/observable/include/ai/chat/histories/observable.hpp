#ifndef AI_CHAT_HISTORIES_OBSERVABLE_HPP
#define AI_CHAT_HISTORIES_OBSERVABLE_HPP

#include <type_traits>
#include <typeinfo>
#include <unordered_map>

namespace ai {
namespace chat {
namespace histories {

template<typename History>
class slot;
template<typename History>
class observable_iterator;
template<typename History>
class observable;

template<typename History>
bool operator<(observable_iterator<History> const &lhs, ::std::chrono::nanoseconds rhs);
template<typename History>
bool operator<(::std::chrono::nanoseconds lhs, observable_iterator<History> const &rhs);

} // histories
} // chat
} // ai

namespace ai {
namespace chat {
namespace histories {

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
    template<typename Action,
        ::std::enable_if_t<
            ::std::is_invocable_v<Action, observable_iterator<History>>,
            bool> = true>
    void on_erase(Action &&callback);
    template<typename Action,
        ::std::enable_if_t<
            ::std::is_invocable_v<Action, observable_iterator<History>, observable_iterator<History>>,
            bool> = true>
    void on_erase(Action &&callback);
    template<typename Action>
    void on_update(Action &&callback);

private:
    friend observable<History>;

    slot(::std::type_info const *observer, observable<History> *target);

    ::std::type_info const *_observer;
    observable<History> *_target;
};

template<typename History>
class observable_iterator : private iterator {
public:
    observable_iterator() = delete;
    observable_iterator(observable_iterator const &other);
    observable_iterator(observable_iterator &&other);

    ~observable_iterator() = default;

    observable_iterator & operator=(observable_iterator const &other) = delete;
    observable_iterator & operator=(observable_iterator &&other) = delete;

    message operator*();
    observable_iterator & operator++();
    bool operator==(observable_iterator const &rhs) const;

    observable_iterator operator+(ptrdiff_t rhs) const;
    observable_iterator operator+(::std::chrono::nanoseconds rhs) const;
    ptrdiff_t operator-(observable_iterator const &rhs) const;

    observable_iterator & operator&=(tag rhs);

    template<typename Client>
    observable_iterator & operator=(::std::string_view rhs);

private:
    friend observable<History>;
    friend bool ::ai::chat::histories::operator<(observable_iterator<History> const &lhs, ::std::chrono::nanoseconds rhs);
    friend bool ::ai::chat::histories::operator<(::std::chrono::nanoseconds lhs, observable_iterator<History> const &rhs);

    template<typename... Args>
    explicit observable_iterator(observable<History> *target,
        Args &&...args);

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

    observable_iterator<History> begin();
    observable_iterator<History> end();

    observable_iterator<History> lower_bound(::std::chrono::nanoseconds timestamp);

    template<typename Client>
    observable_iterator<History> insert(message message);
    template<typename Client>
    observable_iterator<History> erase(observable_iterator<History> pos);
    template<typename Client>
    observable_iterator<History> erase(observable_iterator<History> first, observable_iterator<History> last);

private:
    friend slot<History>;
    friend observable_iterator<History>;

    class subscription;

    ::std::unordered_map<::std::type_info const *, subscription> _subscriptions;
};

} // histories
} // chat
} // ai

#include "impl/observable.ipp"

#endif
