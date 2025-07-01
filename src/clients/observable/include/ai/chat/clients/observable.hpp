#ifndef AI_CHAT_CLIENTS_OBSERVABLE_HPP
#define AI_CHAT_CLIENTS_OBSERVABLE_HPP

#include <typeinfo>
#include <unordered_map>

namespace ai {
namespace chat {
namespace clients {

template<template <typename> class Client>
class observable : public Client<observable<Client>> {
public:
    observable() = delete;
    observable(const observable&) = delete;
    observable(observable&&) = delete;

    ~observable() = default;

    observable& operator=(const observable&) = delete;
    observable& operator=(observable&&) = delete;

    class slot {
        friend observable;

    public:
        slot() = delete;
        slot(const slot&) = delete;
        slot(slot&&);

        ~slot();

        slot& operator=(const slot&) = delete;
        slot& operator=(slot&&);

        template<typename Action>
        void on_message(Action&& callback);

    private:
        slot(const ::std::type_info* p_observer, observable* p_target);

        const ::std::type_info* _p_observer;
        observable* _p_target;
    };

    template<typename... Args>
    explicit observable(Args&& ...args);

    template<typename Observer>
    slot subscribe();

private:
    class subscription;

    friend Client<observable>;

    ::std::unordered_map<const ::std::type_info*, subscription> _subscriptions;

    void on_message(const message& message) const;
};

} // clients
} // chat
} // ai

#include "impl/observable.ipp"

#endif
