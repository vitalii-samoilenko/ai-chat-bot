#ifndef TWITCH_IRC_CLIENT_HPP
#define TWITCH_IRC_CLIENT_HPP

#include <chrono>
#include <functional>
#include <string>
#include <typeinfo>
#include <unordered_map>

namespace twitch {
namespace irc {

struct message {
    ::std::string from;
    ::std::string channel;
    ::std::string content;
};
struct command {
    ::std::string from;
    ::std::string channel;
    ::std::string identifier;
    ::std::string args;
};

struct context_impl;
class context {
public:
    context() = delete;
    context(const context&) = delete;
    context(context&&) = delete;

    ~context() = default;

    context& operator=(const context&) = delete;
    context& operator=(context&&) = delete;

    explicit context(context_impl& impl)
        : _impl{ impl } {

    };

    void send(const message& message);
    
private:
    context_impl& _impl;
};

class subscription {
public:
    subscription() = default;
    subscription(const subscription&) = delete;
    subscription(subscription&&) = delete;

    ~subscription() = default;

    subscription& operator=(const subscription&) = delete;
    subscription& operator=(subscription&&) = delete;

    void on_command(context& context, const command& command) const {
        _on_command(context, command);
    };
    void on_command(const ::std::function<void(context&, const command&)>& callback) {
        _on_command = callback;
    };
    void on_message(context& context, const message& message) const {
        _on_message(context, message);
    };
    void on_message(const ::std::function<void(context&, const message&)>& callback) {
        _on_message = callback;
    };

private:
    ::std::function<void(context&, const command&)> _on_command;
    ::std::function<void(context&, const message&)> _on_message;
};

class client {
public:
    client() = delete;
    client(const client&) = delete;
    client(client&&) = delete;

    ~client() = default;

    client& operator=(const client&) = delete;
    client& operator=(client&&) = delete;

    client(const ::std::string& address, ::std::chrono::milliseconds timeout);

    template<typename Target>
    subscription& subscribe() {
        return _subscriptions[&typeid(Target)];
    };

    template<typename Range>
    bool run(const ::std::string& username, const ::std::string& access_token, const Range& channels) const;

private:
    bool _ssl;
    ::std::string _host;
    ::std::string _port;
    ::std::chrono::milliseconds _timeout;
    ::std::unordered_map<const ::std::type_info*, subscription> _subscriptions;
};

} // irc
} // twitch

#include "impl/client.ipp"

#endif
