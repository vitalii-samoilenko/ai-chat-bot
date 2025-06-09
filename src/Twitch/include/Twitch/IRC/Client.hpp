#ifndef TWITCH_IRC_CLIENT_HPP
#define TWITCH_IRC_CLIENT_HPP

#include <chrono>
#include <functional>
#include <string>
#include <typeinfo>
#include <unordered_map>

namespace Twitch {
namespace IRC {

struct Message {
    ::std::string From;
    ::std::string Channel;
    ::std::string Content;
};
struct Command {
    ::std::string From;
    ::std::string Channel;
    ::std::string Identifier;
    ::std::string Args;
};

struct ContextImpl;
class Context {
public:
    Context() = delete;
    Context(const Context&) = delete;
    Context(Context&&) = delete;

    ~Context() = default;

    Context& operator=(const Context&) = delete;
    Context& operator=(Context&&) = delete;

    Context(ContextImpl& impl)
        : m_impl{ impl } {

    };

    void Send(const Message& message);
    void Disconnect();
    
private:
    ContextImpl& m_impl;
};

class Subscription {
public:
    Subscription() = default;
    Subscription(const Subscription&) = delete;
    Subscription(Subscription&&) = delete;

    ~Subscription() = default;

    Subscription& operator=(const Subscription&) = delete;
    Subscription& operator=(Subscription&&) = delete;

    void OnCommand(Context& context, const Command& command) const {
        m_onCommand(context, command);
    };
    void OnCommand(const ::std::function<void(Context&, const Command&)>& callback) {
        m_onCommand = callback;
    };
    void OnMessage(Context& context, const Message& message) const {
        m_onMessage(context, message);
    };
    void OnMessage(const ::std::function<void(Context&, const Message&)>& callback) {
        m_onMessage = callback;
    };

private:
    ::std::function<void(Context&, const Command&)> m_onCommand;
    ::std::function<void(Context&, const Message&)> m_onMessage;
};

class Client {
public:
    Client() = delete;
    Client(const Client&) = delete;
    Client(Client&&) = delete;

    ~Client() = default;

    Client& operator=(const Client&) = delete;
    Client& operator=(Client&&) = delete;

    Client(const ::std::string& baseAddress, ::std::chrono::milliseconds timeout);

    template<typename Target>
    Subscription& Subscribe() {
        return m_subscriptions[&typeid(Target)];
    };

    template<typename Range>
    bool Run(const ::std::string& username, const ::std::string& accessToken, const Range& channels) const;

private:
    bool m_ssl;
    ::std::string m_host;
    ::std::string m_port;
    ::std::chrono::milliseconds m_timeout;
    ::std::unordered_map<const ::std::type_info*, Subscription> m_subscriptions;
};

} // Auth
} // IRC

#include "impl/Client.ipp"

#endif
