#ifndef AI_CHAT_CLIENTS_TWITCH_IRC_HPP
#define AI_CHAT_CLIENTS_TWITCH_IRC_HPP

#include <chrono>
#include <memory>
#include <string>

namespace ai {
namespace chat {
namespace clients {
namespace twitch {

struct message {
    ::std::string username;
    ::std::string content;
    ::std::string channel;
};

template<typename Handler>
class irc : public Handler {
public:
    irc() = delete;
    irc(const irc&) = delete;
    irc(irc&&) = delete;

    ~irc() = default;

    irc& operator=(const irc&) = delete;
    irc& operator=(irc&&) = delete;

    template<typename... Args>
    irc(const ::std::string& address, ::std::chrono::milliseconds timeout, Args&& ...args);

    void connect(const ::std::string& username, const ::std::string& access_token);
    void disconnect();
    void send(const message& message);

private:
    class connection;

    ::std::unique_ptr<connection> _p_channel;
};

} // twitch
} // clients
} // chat
} // ai

#include "impl/irc.ipp"

#endif
