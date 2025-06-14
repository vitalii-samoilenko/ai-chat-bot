#ifndef TWITCH_IRC_CLIENT_IPP
#define TWITCH_IRC_CLIENT_IPP

#include <stdexcept>

#include "boost/asio/buffer.hpp"
#include "boost/beast.hpp"
#include "boost/xpressive/xpressive.hpp"
#include "boost/url.hpp"

#include "eboost/beast/websocket/client.hpp"

#include "twitch/irc/client.hpp"

namespace twitch {
namespace irc {

client::client(const ::std::string& address, ::std::chrono::milliseconds timeout)
    : _ssl{}
    , _host{}
    , _port{}
    , _timeout{ timeout }
    , _subscriptions{} {
    ::boost::system::result<::boost::urls::url_view> result{ ::boost::urls::parse_uri(address) };
    if (!result.has_value()) {
        throw ::std::invalid_argument{ "invalid uri" };
    }
    ::boost::urls::url_view url{ result.value() };
    if (url.scheme() == "wss") {
        _ssl = true;
    } else if (!(url.scheme() == "ws")) {
        throw ::std::invalid_argument{ "Scheme is not supported" };
    }
    _host = url.host();
    _port = url.has_port()
        ? url.port()
        : _ssl
            ? "443"
            : "80";
};

struct context_impl {
    const bool ssl;
    union {
        ::boost::beast::websocket::stream<::boost::beast::tcp_stream>* p_plain_stream;
        ::boost::beast::websocket::stream<::boost::asio::ssl::stream<::boost::beast::tcp_stream>>* p_secure_stream;
    };
    ::std::string join;
    ::std::string pass;
    ::std::string nick;
    const ::std::string& response;
    ::boost::asio::dynamic_string_buffer<char, ::std::char_traits<char>, ::std::allocator<char>> buffer;
    bool stop;
    bool rerun;
    ::boost::xpressive::mark_tag ping_group;
    ::boost::xpressive::mark_tag username_group;
    ::boost::xpressive::mark_tag privmsg_group;
    ::boost::xpressive::mark_tag channel_group;
    ::boost::xpressive::mark_tag message_group;
    ::boost::xpressive::mark_tag notice_group;
    ::boost::xpressive::mark_tag reason_group;
    ::boost::xpressive::mark_tag reconnect_group;
    ::boost::xpressive::sregex command_group;
    const ::std::unordered_map<const ::std::type_info*, subscription>& subscriptions;
    ::std::function<void(::boost::beast::error_code, size_t)> loop;
};

template<typename Stream>
void send(context_impl& context, Stream& stream, const message& message) {
    ::std::string request{ "PRIVMSG #" + message.channel + " :" + message.content };
    stream.async_write(::boost::asio::buffer(request), [&](::boost::beast::error_code error_code, size_t bytes_transferred)->void {
    ::boost::ignore_unused(bytes_transferred);
    ::eboost::beast::ensure_success(error_code);

    }); // PRIVMSG
};
void context::send(const message& message) {
    if (_impl.ssl) {
        ::twitch::irc::send(_impl, *_impl.p_secure_stream, message);
    } else {
        ::twitch::irc::send(_impl, *_impl.p_plain_stream, message);
    }
};

template<typename Stream>
void callback(context_impl& context, Stream& stream) {
    stream.async_write(::boost::asio::buffer(context.pass), [&](::boost::beast::error_code error_code, size_t bytes_transferred)->void {
    ::boost::ignore_unused(bytes_transferred);
    ::eboost::beast::ensure_success(error_code);

    stream.async_write(::boost::asio::buffer(context.nick), [&](::boost::beast::error_code error_code, size_t bytes_transferred)->void {
    ::boost::ignore_unused(bytes_transferred);
    ::eboost::beast::ensure_success(error_code);

    stream.async_read(context.buffer, [&](::boost::beast::error_code error_code, size_t bytes_transferred)->void {
    ::eboost::beast::ensure_success(error_code);

    ::boost::xpressive::smatch what{};
    if (::boost::xpressive::regex_match(context.response, what, context.command_group)
        && what[context.notice_group]) {
        throw ::std::invalid_argument{ what[context.reason_group] };
    }
    context.buffer.consume(bytes_transferred);

    stream.async_write(::boost::asio::buffer(context.join), [&](::boost::beast::error_code error_code, size_t bytes_transferred)->void {
    ::boost::ignore_unused(bytes_transferred);
    ::eboost::beast::ensure_success(error_code);

    context.loop = [&](::boost::beast::error_code error_code, size_t bytes_transferred)->void {
        ::eboost::beast::ensure_success(error_code);

        for (::boost::xpressive::sregex_iterator current{ context.response.begin(), context.response.end(), context.command_group }, end{}; current != end; ++current) {
            const ::boost::xpressive::smatch& what{ *current };
            if (what[context.ping_group]) {
                ::std::string request{ "PONG :tmi.twitch.tv" };
                stream.async_write(::boost::asio::buffer(request), [&](::boost::beast::error_code error_code, size_t bytes_transferred)->void {
                ::boost::ignore_unused(bytes_transferred);
                ::eboost::beast::ensure_success(error_code);

                }); // PONG
            } else if (what[context.privmsg_group]) {
                message message{ what[context.username_group], what[context.channel_group], what[context.message_group] };
                for (const auto& target_subscription : context.subscriptions) {
                    const subscription& subscription{ target_subscription.second };
                    ::twitch::irc::context wrapper{ context };
                    subscription.on_message(wrapper, message);
                }
            } else if (what[context.reconnect_group]) {
                context.rerun = true;
                context.stop = true;
            }
        }
        context.buffer.consume(bytes_transferred);

        if (context.stop) {
            // Gracefully close the stream
            stream.async_close(::boost::beast::websocket::close_code::normal, [&](::boost::beast::error_code error_code)->void {
            ::eboost::beast::ensure_success(error_code);

            // If we get here then the connection is closed gracefully

            }); // shutdown
        } else {
            stream.async_read(context.buffer, context.loop);
        }
    };
    stream.async_read(context.buffer, context.loop);

    }); // JOIN
    }); // NOTICE
    }); // NICK
    }); // PASS
};

template<typename Range>
bool client::run(const ::std::string& username, const ::std::string& access_token, const Range& channels) const {
    ::std::string response{};
    context_impl context{
        _ssl, {},
        "JOIN ", "PASS oauth:" + access_token, "NICK " + username,
        response, ::boost::asio::dynamic_string_buffer<char, ::std::char_traits<char>, ::std::allocator<char>>{ response },
        false, false,
        { 1 }, { 2 }, { 3 }, { 4 },
        { 5 }, { 6 }, { 7 }, { 8 },
        {},
        _subscriptions, {}
    };
    for (const ::std::string& channel : channels) {
        context.join += '#' + channel + ',';
    }
    context.join.resize(context.join.size() - 1);
    context.command_group =
        ((context.ping_group = "PING") >> " :tmi.twitch.tv" >> ::boost::xpressive::_ln)
        | (':' >> (context.username_group = +::boost::xpressive::range('a', 'z')) >> '!' >> context.username_group >> '@' >> context.username_group >> ".tmi.twitch.tv "
            >> (context.privmsg_group = "PRIVMSG") >> " #" >> (context.channel_group = +::boost::xpressive::range('a', 'z')) >> " :" >> (context.message_group = +~::boost::xpressive::_ln) >> ::boost::xpressive::_ln)
        | (":tmi.twitch.tv " >> (context.notice_group = "NOTICE") >> " * :" >> (context.reason_group = +~::boost::xpressive::_ln) >> ::boost::xpressive::_ln)
        | (":tmi.twitch.tv " >> (context.reconnect_group = "RECONNECT"));

    if (_ssl) {
        ::eboost::beast::websocket::client::run(
            ::eboost::beast::secure_channel_tag{},
            _host, _port,
            _timeout,
            [&](::boost::beast::websocket::stream<::boost::asio::ssl::stream<::boost::beast::tcp_stream>>& stream)->void {
                context.p_secure_stream = &stream;
                callback(context, stream);
            });
    } else {
        ::eboost::beast::websocket::client::run(
            ::eboost::beast::plain_channel_tag{},
            _host, _port,
            _timeout,
            [&](::boost::beast::websocket::stream<::boost::beast::tcp_stream>& stream)->void {
                context.p_plain_stream = &stream;
                callback(context, stream);
            });
    }
    return context.rerun;
};

} // irc
} // twitch

#endif
