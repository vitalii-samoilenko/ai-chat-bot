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
    public:
        slot() = delete;
        slot(const slot&) = delete;
        slot(slot&&);

        ~slot();

        slot& operator=(const slot&) = delete;
        slot& operator=(slot&&);

        template<typename Action>
        void on_message(Action&& callback);
        template<typename Action>
        void on_command(Action&& callback);

    private:
        friend observable;

        slot(const ::std::type_info* observer, observable* target);

        const ::std::type_info* _observer;
        observable* _target;
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
    void on_command(const command& command) const;
};

} // clients
} // chat
} // ai

#include "impl/observable.ipp"

#endif
