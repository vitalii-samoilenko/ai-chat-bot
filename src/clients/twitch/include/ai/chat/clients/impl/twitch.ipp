#ifndef AI_CHAT_CLIENTS_TWITCH_IPP
#define AI_CHAT_CLIENTS_TWITCH_IPP

#include <stdexcept>
#include <utility>

#include "re2/re2.h"

#include "ai/chat/clients/twitch.hpp"

namespace ai {
namespace chat {
namespace clients {

template<typename Handler>
twitch<Handler>::twitch(::std::string_view address, ::std::chrono::milliseconds timeout,
    ::std::chrono::milliseconds delay, size_t dop)
    : _channel{ dop, *this } {
    ::RE2 uri{ R"(wss:\/\/(?<host>[^:\/]+)(?::(?<port>\d+))?(?<path>\/.*))" };
    if (!::RE2::FullMatch(address, uri,
            &_channel._host, &_channel._port, &_channel._path)) {
        throw ::std::invalid_argument{ "invalid uri" };
    }
    if (_channel._port.empty()) {
        _channel._port = "443";
    }
    _channel._timeout = timeout;
    _channel._delay = delay;
    _channel.on_init();
};

template<typename Handler>
void twitch<Handler>::connect(::std::string_view username, ::std::string_view access_token) {
    START_SPAN(span, "connect", _channel)
    ::boost::asio::post(_channel._io_context, [this,
        username = ::std::string{ username },
        access_token = ::std::string{ access_token }
        PROPAGATE_SPAN(span)]()->void {
    _channel._username = ::std::move(username);
    _channel._access_token = ::std::move(access_token);
    _channel.on_connect(
        PROPAGATE_ONLY_SPAN(span));

    }); // post
};
template<typename Handler>
void twitch<Handler>::disconnect() {
    START_SPAN(span, "disconnect", _channel)
    ::boost::asio::post(_channel._io_context, [this
        PROPAGATE_SPAN(span)]()->void {
    _channel.on_disconnect(
        PROPAGATE_ONLY_SPAN(span));

    }); // post
    _channel._io_context.wait();
};
template<typename Handler>
void twitch<Handler>::send(message message) {
    START_SPAN(span, "send", _channel)
    ::boost::asio::post(_channel._io_context, [this,
        message_content = ::std::string{ message.content },
        message_channel = ::std::string{ message.channel }
        PROPAGATE_SPAN(span)]()->void {
    _channel._message_content = ::std::move(message_content);
    _channel._message_channel = ::std::move(message_channel);
    _channel.on_send(
        PROPAGATE_ONLY_SPAN(span));

    }); // post
};

template<typename Handler>
void twitch<Handler>::join(::std::string_view channel) {
    START_SPAN(span, "join", _channel)
    ::boost::asio::post(_channel._io_context, [this,
        channel = ::std::string{ channel }
        PROPAGATE_SPAN(span)]()->void {
    _channel._channel = ::std::move(channel);
    _channel.on_join(
        PROPAGATE_ONLY_SPAN(span));

    }); // post
};
template<typename Handler>
void twitch<Handler>::leave() {
    START_SPAN(span, "leave", _channel)
    ::boost::asio::post(_channel._io_context, [this
        PROPAGATE_SPAN(span)]()->void {
    _channel.on_leave(
        PROPAGATE_ONLY_SPAN(span));

    }); // post
};
template<typename Handler>
void twitch<Handler>::attach() {
    _channel._handler_context.attach();
};

template<typename Handler>
void twitch<Handler>::on_message(message const &message) const {
    static_cast<const Handler *>(this)->on_message(message);
};
template<typename Handler>
void twitch<Handler>::on_command(command const &command) const {
    static_cast<const Handler *>(this)->on_command(command);
};

} // clients
} // chat
} // ai

#endif
