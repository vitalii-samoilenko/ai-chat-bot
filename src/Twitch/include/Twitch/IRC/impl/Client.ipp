#ifndef TWITCH_IRC_CLIENT_IPP
#define TWITCH_IRC_CLIENT_IPP

#include <stdexcept>

#include "boost/asio/buffer.hpp"
#include "boost/beast.hpp"
#include "boost/xpressive/xpressive.hpp"
#include "boost/url.hpp"

#include "eboost/beast/websocket/client.hpp"

#include "Twitch/IRC/Client.hpp"

namespace Twitch {
namespace IRC {

Client::Client(const ::std::string& baseAddress, ::std::chrono::milliseconds timeout)
    : m_ssl{}
    , m_host{}
    , m_port{}
    , m_timeout{ timeout }
    , m_subscriptions{} {
    ::boost::system::result<::boost::urls::url_view> result{ ::boost::urls::parse_uri(baseAddress) };
    if (!result.has_value()) {
        throw ::std::invalid_argument{ "Invalid URI" };
    }
    ::boost::urls::url_view url{ result.value() };
    if (url.scheme() == "wss") {
        m_ssl = true;
    } else if (!(url.scheme() == "ws")) {
        throw ::std::invalid_argument{ "Scheme is not supported" };
    }
    m_host = url.host();
    m_port = url.has_port()
        ? url.port()
        : m_ssl
            ? "443"
            : "80";
};

struct ContextImpl {
    const bool ssl;
    union {
        ::boost::beast::websocket::stream<::boost::beast::tcp_stream>* pPlainStream;
        ::boost::beast::websocket::stream<::boost::asio::ssl::stream<::boost::beast::tcp_stream>>* pSecureStream;
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
    const ::std::unordered_map<const ::std::type_info*, Subscription>& subscriptions;
    ::std::function<void(::boost::beast::error_code, size_t)> loop;
};

template<typename Stream>
void send(ContextImpl& context, Stream& stream, const Message& message) {
    ::std::string request{ "PRIVMSG #" + message.Channel + " :" + message.Content };
    stream.async_write(::boost::asio::buffer(request), [&](::boost::beast::error_code errorCode, size_t transferredBytes)->void {
    ::boost::ignore_unused(transferredBytes);
    ::eboost::beast::ensure_success(errorCode);

    }); // PRIVMSG
};
void Context::Send(const Message& message) {
    if (m_impl.ssl) {
        send(m_impl, *m_impl.pSecureStream, message);
    } else {
        send(m_impl, *m_impl.pPlainStream, message);
    }
};

template<typename Stream>
void callback(ContextImpl& context, Stream& stream) {
    stream.async_write(::boost::asio::buffer(context.pass), [&](::boost::beast::error_code errorCode, size_t transferredBytes)->void {
    ::boost::ignore_unused(transferredBytes);
    ::eboost::beast::ensure_success(errorCode);

    stream.async_write(::boost::asio::buffer(context.nick), [&](::boost::beast::error_code errorCode, size_t transferredBytes)->void {
    ::boost::ignore_unused(transferredBytes);
    ::eboost::beast::ensure_success(errorCode);

    stream.async_read(context.buffer, [&](::boost::beast::error_code errorCode, size_t transferredBytes)->void {
    ::eboost::beast::ensure_success(errorCode);

    ::boost::xpressive::smatch what{};
    if (::boost::xpressive::regex_match(context.response, what, context.command_group)
        && what[context.notice_group]) {
        throw ::std::invalid_argument{ what[context.reason_group] };
    }
    context.buffer.consume(transferredBytes);

    stream.async_write(::boost::asio::buffer(context.join), [&](::boost::beast::error_code errorCode, size_t transferredBytes)->void {
    ::boost::ignore_unused(transferredBytes);
    ::eboost::beast::ensure_success(errorCode);

    context.loop = [&](::boost::beast::error_code errorCode, size_t transferredBytes)->void {
        ::eboost::beast::ensure_success(errorCode);

        for (::boost::xpressive::sregex_iterator current{ context.response.begin(), context.response.end(), context.command_group }, end{}; current != end; ++current) {
            const ::boost::xpressive::smatch& what{ *current };
            if (what[context.ping_group]) {
                ::std::string request{ "PONG :tmi.twitch.tv" };
                stream.async_write(::boost::asio::buffer(request), [&](::boost::beast::error_code errorCode, size_t transferredBytes)->void {
                ::boost::ignore_unused(transferredBytes);
                ::eboost::beast::ensure_success(errorCode);

                }); // PONG
            } else if (what[context.privmsg_group]) {
                Message message{ what[context.username_group], what[context.channel_group], what[context.message_group] };
                for (const auto& target_subscription : context.subscriptions) {
                    const Subscription& subscription{ target_subscription.second };
                    Context wrapper{ context };
                    subscription.OnMessage(wrapper, message);
                }
            } else if (what[context.reconnect_group]) {
                context.rerun = true;
                context.stop = true;
            }
        }
        context.buffer.consume(transferredBytes);

        if (context.stop) {
            // Gracefully close the stream
            stream.async_close(::boost::beast::websocket::close_code::normal, [&](::boost::beast::error_code errorCode)->void {
            ::eboost::beast::ensure_success(errorCode);

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
bool Client::Run(const ::std::string& username, const ::std::string& accessToken, const Range& channels) const {
    ::std::string response{};
    ContextImpl context{
        m_ssl, {},
        "JOIN ", "PASS oauth:" + accessToken, "NICK " + username,
        response, ::boost::asio::dynamic_string_buffer<char, ::std::char_traits<char>, ::std::allocator<char>>{ response },
        false, false,
        { 1 }, { 2 }, { 3 }, { 4 },
        { 5 }, { 6 }, { 7 }, { 8 },
        {},
        m_subscriptions, {}
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

    if (m_ssl) {
        ::eboost::beast::websocket::client::run(
            ::eboost::beast::secure_channel_tag{},
            m_host, m_port,
            m_timeout,
            [&](::boost::beast::websocket::stream<::boost::asio::ssl::stream<::boost::beast::tcp_stream>>& stream)->void {
                context.pSecureStream = &stream;
                callback(context, stream);
            });
    } else {
        ::eboost::beast::websocket::client::run(
            ::eboost::beast::plain_channel_tag{},
            m_host, m_port,
            m_timeout,
            [&](::boost::beast::websocket::stream<::boost::beast::tcp_stream>& stream)->void {
                context.pPlainStream = &stream;
                callback(context, stream);
            });
    }
    return context.rerun;
};

} // Auth
} // Twitch

#endif
