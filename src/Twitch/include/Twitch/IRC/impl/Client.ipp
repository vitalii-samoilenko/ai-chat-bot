#ifndef TWITCH_IRC_CLIENT_IPP
#define TWITCH_IRC_CLIENT_IPP

#include <stdexcept>

#include "boost/asio/ssl.hpp"
#include "boost/asio/strand.hpp"
#include "boost/asio/buffer.hpp"
#include "boost/beast.hpp"
#include "boost/beast/ssl.hpp"
#include "boost/xpressive/xpressive.hpp"
#include "boost/url.hpp"

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
        throw ::std::invalid_argument{ "baseAddress" };
    }
    ::boost::urls::url_view url{ result.value() };
    if (url.scheme() == "wss") {
        m_ssl = true;
    } else if (!(url.scheme() == "ws")) {
        throw ::std::invalid_argument{ "baseAddress" };
    }
    m_host = url.host();
    m_port = url.has_port()
        ? url.port()
        : m_ssl
            ? "443"
            : "80";
};

void ensureSuccess(::boost::beast::error_code errorCode) {
    if (errorCode) {
        throw ::boost::beast::system_error{ errorCode } ;
    }
};

struct ContextImpl {
    const bool ssl;
    union {
        ::std::reference_wrapper<::boost::beast::websocket::stream<::boost::beast::tcp_stream>> stream;
        ::std::reference_wrapper<::boost::beast::websocket::stream<::boost::asio::ssl::stream<::boost::beast::tcp_stream>>> secureStream;
    };
    ::std::string& request;
    bool& stop;
};

void Context::Send(const Message& message) {
    m_impl.request = "PRIVMSG #" + message.Channel + " :" + message.Content;
    if (m_impl.ssl) {
        ::boost::beast::websocket::stream<::boost::asio::ssl::stream<::boost::beast::tcp_stream>>& stream{ m_impl.secureStream.get() };
        stream.async_write(::boost::asio::buffer(m_impl.request), [&](::boost::beast::error_code errorCode, size_t transferredBytes)->void {
        ::boost::ignore_unused(transferredBytes);
        ensureSuccess(errorCode);

        }); // PRIVMSG
    } else {
        ::boost::beast::websocket::stream<::boost::beast::tcp_stream>& stream{ m_impl.stream.get() };
        stream.async_write(::boost::asio::buffer(m_impl.request), [&](::boost::beast::error_code errorCode, size_t transferredBytes)->void {
        ::boost::ignore_unused(transferredBytes);
        ensureSuccess(errorCode);

        }); // PRIVMSG
    }
};
void Context::Disconnect() {
    m_impl.stop = true;
};

struct secure_channel_tag{};
struct plain_channel_tag{};

template<typename Range>
bool run(secure_channel_tag,
    const ::std::string& host, const ::std::string& port,
    ::std::chrono::milliseconds timeout,
    const ::std::string& username, const ::std::string& accessToken, const Range& channels,
    const ::std::unordered_map<const ::std::type_info*, Subscription>& subscriptions) {
    ::boost::asio::io_context ioContext{};

    ::boost::asio::ip::tcp::resolver resolver{ ::boost::asio::make_strand(ioContext) };

    ::boost::asio::ssl::context sslContext{ ::boost::asio::ssl::context::tlsv12_client };
    sslContext.set_verify_mode(::boost::asio::ssl::verify_none);

    ::boost::beast::websocket::stream<::boost::asio::ssl::stream<::boost::beast::tcp_stream>> stream{ ::boost::asio::make_strand(ioContext), sslContext };
    if (!::SSL_set_tlsext_host_name(stream.next_layer().native_handle(), host.c_str())) {
        throw ::boost::beast::system_error{
            static_cast<int>(::ERR_get_error()),
            ::boost::asio::error::get_ssl_category()
        };
    }
    stream.next_layer().set_verify_callback(::boost::asio::ssl::host_name_verification(host));

    ::std::string request{};
    ::std::string response{};
    bool stop{ false };
    bool reconnect{ false };
    
    ::boost::asio::dynamic_string_buffer buffer{ response };

    ContextImpl clientContextImpl{ true, {}, request, stop };
    clientContextImpl.secureStream = stream;
    Context clientContext{ clientContextImpl };

    ::boost::xpressive::mark_tag tags_group{ 1 };
    ::boost::xpressive::mark_tag ping_group{ 2 };
    ::boost::xpressive::mark_tag username_group{ 3 };
    ::boost::xpressive::mark_tag command_group{ 4 };
    ::boost::xpressive::mark_tag args_group{ 5 };
    ::boost::xpressive::sregex message_group{ 
        //::boost::xpressive::sregex::compile(R"(^(?:@(?<tags>\S+) )?(?:(?<ping>PING) )?:(?:(?<username>[a-z]+)!\g<username>@\g<username>[.])?tmi[.]twitch[.]tv(?: (?<command>[A-Z]+)(?: (?<args>.+))*)?$)")
        !('@' >> (tags_group = +~::boost::xpressive::_s) >> ' ')
        >> !((ping_group = "PING") >> ' ')
        >> ':'
            >> !((username_group = +::boost::xpressive::range('a', 'z')) >> '!' >> username_group >> '@' >> username_group >> '.')
            >> "tmi.twitch.tv"
            >> !(' ' >> (command_group = +::boost::xpressive::range('A', 'Z')) >> *(' ' >> (args_group = +::boost::xpressive::_)))
    };

    ::std::function<void(::boost::beast::error_code, size_t)> loop = [&](::boost::beast::error_code errorCode, size_t transferredBytes)->void {
        ensureSuccess(errorCode);

        for (::boost::xpressive::sregex_iterator current( response.begin(), response.end(), message_group ), end{}; current != end; ++current ) {
            const ::boost::xpressive::smatch& what{ *current };
            if (what[ping_group] == "PING") {
                request = "PONG :tmi.twitch.tv";
                stream.async_write(::boost::asio::buffer(request), [&](::boost::beast::error_code errorCode, size_t transferredBytes)->void {
                ::boost::ignore_unused(transferredBytes);
                ensureSuccess(errorCode);

                }); // PONG
            } else {
                const ::std::string& identifier{ what[command_group] };
                if (identifier == "PRIVMSG") {
                    Message message{ what[username_group], "", what[args_group] };
                    for (const auto& target_subscription : subscriptions) {
                        const Subscription& subscription{ target_subscription.second };
                        subscription.OnMessage(clientContext, message);
                    }
                } else if (identifier == "RECONNECT") {
                    reconnect = true;
                    stop = true;
                }
            }
        }
        buffer.consume(transferredBytes);

        if (stop) {
            // Gracefully close the stream
            stream.async_close(::boost::beast::websocket::close_code::normal, [&](::boost::beast::error_code errorCode)->void {
            ensureSuccess(errorCode);

            // If we get here then the connection is closed gracefully

            }); // shutdown
        } else {
            stream.async_read(buffer, loop);
        }
    };

    // Set a timeout on the operation
    ::boost::beast::get_lowest_layer(stream).expires_after(timeout);

     // Look up the domain name
    resolver.async_resolve(host, port, [&](::boost::beast::error_code errorCode, ::boost::asio::ip::tcp::resolver::results_type results)->void {
    ensureSuccess(errorCode);

    // Make the connection on the IP address we get from a lookup
    ::boost::beast::get_lowest_layer(stream).async_connect(results, [&](::boost::beast::error_code errorCode, ::boost::asio::ip::tcp::resolver::results_type::endpoint_type)->void {
    ensureSuccess(errorCode);

    // Perform the SSL handshake
    stream.next_layer().async_handshake(::boost::asio::ssl::stream_base::client, [&](::boost::beast::error_code errorCode)->void {
    ensureSuccess(errorCode);

    ::boost::beast::get_lowest_layer(stream).expires_never();
    stream.set_option(
        ::boost::beast::websocket::stream_base::timeout::suggested(
            ::boost::beast::role_type::client));

    stream.async_handshake(host + ":" + port, "/", [&](::boost::beast::error_code errorCode)->void {
    ensureSuccess(errorCode);

    request = "PASS oauth:" + accessToken;
    stream.async_write(::boost::asio::buffer(request), [&](::boost::beast::error_code errorCode, size_t transferredBytes)->void {
    ::boost::ignore_unused(transferredBytes);
    ensureSuccess(errorCode);

    request = "NICK " + username;
    stream.async_write(::boost::asio::buffer(request), [&](::boost::beast::error_code errorCode, size_t transferredBytes)->void {
    ::boost::ignore_unused(transferredBytes);
    ensureSuccess(errorCode);

    stream.async_read(buffer, [&](::boost::beast::error_code errorCode, size_t transferredBytes)->void {
    ensureSuccess(errorCode);

    ::boost::xpressive::smatch what{};
    if (::boost::xpressive::regex_match(response, what, message_group)
        && what[command_group] == "NOTICE") {
        return;
    }
    buffer.consume(transferredBytes);

    request = "JOIN ";
    for (const ::std::string& channel : channels) {
        request += '#' + channel + ',';
    }
    request.resize(request.size() - 1);
    stream.async_write(::boost::asio::buffer(request), [&](::boost::beast::error_code errorCode, size_t transferredBytes)->void {
    ::boost::ignore_unused(transferredBytes);
    ensureSuccess(errorCode);

    stream.async_read(buffer, loop);
    
    }); // JOIN
    }); // Auth
    }); // NICK
    }); // PASS
    }); // WS handshake
    }); // SSL handshake
    }); // connect
    }); // resolve

    ioContext.run();

    return reconnect;
};
template<typename Range>
bool run(plain_channel_tag,
    const ::std::string& host, const ::std::string& port,
    ::std::chrono::milliseconds timeout,
    const ::std::string& username, const ::std::string& accessToken, const Range& channels,
    const ::std::unordered_map<const ::std::type_info*, Subscription>& subscriptions) {
    return false;
};

template<typename Range>
bool Client::Run(const ::std::string& username, const ::std::string& accessToken, const Range& channels) const {
    if (m_ssl) {
        return run<Range>(secure_channel_tag{},
            m_host, m_port,
            m_timeout,
            username, accessToken, channels,
            m_subscriptions);
    }
    return run<Range>(plain_channel_tag{},
        m_host, m_port,
        m_timeout,
        username, accessToken, channels,
        m_subscriptions);
};

} // Auth
} // Twitch

#endif
