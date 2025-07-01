#ifndef AI_CHAT_CLIENTS_TWITCH_HPP
#define AI_CHAT_CLIENTS_TWITCH_HPP

#include <chrono>
#include <memory>
#include <string>

namespace ai {
namespace chat {
namespace clients {

struct message {
    ::std::string username;
    ::std::string content;
    ::std::string channel;
};

template<typename Handler>
class twitch {
public:
    twitch() = delete;
    twitch(const twitch&) = delete;
    twitch(twitch&&) = delete;

    ~twitch() = default;

    twitch& operator=(const twitch&) = delete;
    twitch& operator=(twitch&&) = delete;

    twitch(size_t dop,
        const ::std::string& address, ::std::chrono::milliseconds timeout);

    void connect(const ::std::string& username, const ::std::string& access_token);
    void disconnect();
    void send(const message& message);

    void join(const ::std::string& channel);
    void leave();
    void attach();
private:
    class connection;
    friend connection;

    ::std::unique_ptr<connection> _p_channel;

    void on_message(const message& message) const;
};

} // clients
} // chat
} // ai

#include "impl/twitch.ipp"

#endif
