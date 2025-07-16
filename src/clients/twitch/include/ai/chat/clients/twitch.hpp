#ifndef AI_CHAT_CLIENTS_TWITCH_HPP
#define AI_CHAT_CLIENTS_TWITCH_HPP

#include <chrono>
#include <string_view>

namespace ai {
namespace chat {
namespace clients {

struct message {
    ::std::string_view username;
    ::std::string_view content;
    ::std::string_view channel;
};
struct command {
    ::std::string_view username;
    ::std::string_view name;
    ::std::string_view args;
    ::std::string_view channel;
};

template<typename Handler>
class twitch;

} // clients
} // chat
} // ai

#include "detail/connection.hpp"

namespace ai {
namespace chat {
namespace clients {

template<typename Handler>
class twitch {
public:
    twitch() = delete;
    twitch(twitch const &other) = delete;
    twitch(twitch &&other) = delete;

    ~twitch() = default;

    twitch & operator=(twitch const &other) = delete;
    twitch & operator=(twitch &&other) = delete;

    twitch(::std::string_view address, ::std::chrono::milliseconds timeout,
        ::std::chrono::milliseconds delay, size_t dop);

    void connect(::std::string_view username, ::std::string_view access_token);
    void disconnect();
    void send(message message);

    void join(::std::string_view channel);
    void leave();
    void attach();
private:
    friend detail::connection<Handler>;

    detail::connection<Handler> _channel;

    void on_message(message const &message) const;
    void on_command(command const &command) const;
};

} // clients
} // chat
} // ai

#include "impl/twitch.ipp"

#endif
