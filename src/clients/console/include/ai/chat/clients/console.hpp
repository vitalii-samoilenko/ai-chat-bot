#ifndef AI_CHAT_CLIENTS_CONSOLE_HPP
#define AI_CHAT_CLIENTS_CONSOLE_HPP

#include <chrono>
#include <string_view>

namespace ai {
namespace chat {
namespace clients {

struct message {
    ::std::string_view username;
    ::std::string_view content;
};
struct command {
    ::std::string_view username;
    ::std::string_view name;
    ::std::string_view args;
};

template<typename Handler>
class console;

} // clients
} // chat
} // ai

#include "detail/connection.hpp"

namespace ai {
namespace chat {
namespace clients {

template<typename Handler>
class console {
public:
    // console() = delete;
    console(console const &other) = delete;
    console(console &&other) = delete;

    ~console() = default;

    console & operator=(console const &other) = delete;
    console & operator=(console &&other) = delete;

    console();

    void connect(::std::string_view username);
    void disconnect();
    void send(message message);

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

#include "impl/console.ipp"

#endif
