#ifndef AI_CHAT_CLIENTS_OBSERVABLE_HPP
#define AI_CHAT_CLIENTS_OBSERVABLE_HPP

#include <typeinfo>
#include <unordered_map>

namespace ai {
namespace chat {
namespace clients {

template<template <typename> class Client>
class observable;

template<template <typename> class Client>
class slot {
public:
    slot() = delete;
    slot(slot const &other) = delete;
    slot(slot &&);

    ~slot();

    slot & operator=(slot const &other) = delete;
    slot & operator=(slot &&other) = delete;

    template<typename Action>
    void on_message(Action &&callback);
    template<typename Action>
    void on_command(Action &&callback);

private:
    friend observable<Client>;

    slot(::std::type_info const *observer, observable<Client> *target);

    ::std::type_info const *_observer;
    observable<Client> *_target;
};

template<template <typename> class Client>
class observable : public Client<observable<Client>> {
public:
    observable() = delete;
    observable(observable const &other) = delete;
    observable(observable const &&other) = delete;

    ~observable() = default;

    observable & operator=(observable const &other) = delete;
    observable & operator=(observable &&other) = delete;

    template<typename... Args>
    explicit observable(Args &&...args);

    template<typename Observer>
    slot<Client> subscribe();

private:
    friend Client<observable>;
    friend slot<Client>;

    class subscription;

    ::std::unordered_map<::std::type_info const *, subscription> _subscriptions;

    void on_message(message const &message) const;
    void on_command(command const &command) const;
};

} // clients
} // chat
} // ai

#include "impl/observable.ipp"

#endif
