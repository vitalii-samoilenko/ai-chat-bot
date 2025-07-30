#ifndef AI_CHAT_CLIENTS_CONSOLE_IPP
#define AI_CHAT_CLIENTS_CONSOLE_IPP

#include <stdexcept>

#include "ai/chat/clients/console.hpp"

namespace ai {
namespace chat {
namespace clients {

template<typename Handler>
console<Handler>::console()
    : _channel{ *this } {
    _channel.on_init();
};

template<typename Handler>
void console<Handler>::connect(::std::string_view username) {
    START_SPAN(span, "connect", _channel)
    _channel._username = username;
    _channel.on_connect(
        PROPAGATE_ONLY_SPAN(span));
};
template<typename Handler>
void console<Handler>::disconnect() {
    START_SPAN(span, "disconnect", _channel)
    _channel.on_disconnect(
        PROPAGATE_ONLY_SPAN(span));
};
template<typename Handler>
void console<Handler>::send(message message) {
    START_SPAN(span, "send", _channel)
    _channel._message_content = message.content;
    _channel.on_send(
        PROPAGATE_ONLY_SPAN(span));
};

template<typename Handler>
void console<Handler>::attach() {
    for (;;) {
        _channel.on_read();
    }
};

template<typename Handler>
void console<Handler>::on_message(message const &message) const {
    static_cast<const Handler *>(this)->on_message(message);
};
template<typename Handler>
void console<Handler>::on_command(command const &command) const {
    static_cast<const Handler *>(this)->on_command(command);
};

} // clients
} // chat
} // ai

#endif
