#ifndef AI_CHAT_CLIENTS_TWITCH_IPP
#define AI_CHAT_CLIENTS_TWITCH_IPP

#include <stdexcept>
#include <utility>

#include "re2/re2.h"

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
    ::opentelemetry::nostd::shared_ptr<::opentelemetry::trace::Span> span{
        _channel._tracer->StartSpan("connect")
    };
    ::boost::asio::post(_channel._io_context, [this,
        username = ::std::string{ username },
        access_token = ::std::string{ access_token },
        span]()->void {
    _channel._username = ::std::move(username);
    _channel._access_token = ::std::move(access_token);
    _channel.on_connect(
        span);

    }); // post
};
template<typename Handler>
void twitch<Handler>::disconnect() {
    ::opentelemetry::nostd::shared_ptr<::opentelemetry::trace::Span> span{
        _channel._tracer->StartSpan("disconnect")
    };
    ::boost::asio::post(_channel._io_context, [this, span]()->void {
    _channel.on_disconnect(
        span);

    }); // post
    _channel._io_context.wait();
};
template<typename Handler>
void twitch<Handler>::send(message message) {
    ::opentelemetry::nostd::shared_ptr<::opentelemetry::trace::Span> span{
        _channel._tracer->StartSpan("send")
    };
    ::boost::asio::post(_channel._io_context, [this,
        message_content = ::std::string{ message.content },
        message_channel = ::std::string{ message.channel },
        span]()->void {
    _channel._message_content = ::std::move(message_content);
    _channel._message_channel = ::std::move(message_channel);
    _channel.on_send(
        span);

    }); // post
};

template<typename Handler>
void twitch<Handler>::join(::std::string_view channel) {
    ::opentelemetry::nostd::shared_ptr<::opentelemetry::trace::Span> span{
        _channel._tracer->StartSpan("join")
    };
    ::boost::asio::post(_channel._io_context, [this,
        channel = ::std::string{ channel },
        span]()->void {
    _channel._channel = ::std::move(channel);
    _channel.on_join(
        span);

    }); // post
};
template<typename Handler>
void twitch<Handler>::leave() {
    ::opentelemetry::nostd::shared_ptr<::opentelemetry::trace::Span> span{
        _channel._tracer->StartSpan("leave")
    };
    ::boost::asio::post(_channel._io_context, [this, span]()->void {
    _channel.on_leave(
        span);
    _channel._channel.clear();

    }); // post
};
template<typename Handler>
void twitch<Handler>::attach() {
    _channel._h_context.attach();
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
