#ifndef AI_CHAT_CLIENTS_TWITCH_IRC_IPP
#define AI_CHAT_CLIENTS_TWITCH_IRC_IPP

#include <stdexcept>
#include <utility>

#include "boost/asio/buffer.hpp"
#include "boost/asio/buffers_iterator.hpp"
#include "boost/asio/thread_pool.hpp"
#include "boost/asio/ssl.hpp"
#include "boost/beast.hpp"
#include "boost/beast/ssl.hpp"
#include "boost/url.hpp"
#include "boost/xpressive/xpressive.hpp"

#include "ai/chat/clients/twitch/irc.hpp"

namespace ai {
namespace chat {
namespace clients {
namespace twitch {

template<typename Handler>
class irc<Handler>::connection {
    friend irc;

public:
    connection() = delete;
    connection(const connection&) = delete;
    connection(connection&&) = delete;

    ~connection() = default;

    connection& operator=(const connection&) = delete;
    connection& operator=(connection&&) = delete;

private:
    explicit connection(Handler& handler)
        : _context{ 1 }
        , _resolver{ _context }
        , _ssl_context{ ::boost::asio::ssl::context::tlsv12_client }
        , _stream{ _context, _ssl_context }
        , _handler{ handler }
        , _host{}
        , _port{}
        , _timeout{}
        , _authority{}
        , _buffer{}
        , _ping_group{ 1 }
        , _username_group{ 2 }
        , _privmsg_group{ 3 }
        , _channel_group{ 4 }
        , _message_group{ 5 }
        , _notice_group{ 6 }
        , _reason_group{ 7 }
        , _reconnect_group{ 8 }
        , _command_group{
            ((_ping_group = "PING") >> " :tmi.twitch.tv" >> ::boost::xpressive::_ln)
            | (':' >> (_username_group = +::boost::xpressive::range('a', 'z')) >> '!' >> _username_group >> '@' >> _username_group >> ".tmi.twitch.tv "
                >> (_privmsg_group = "PRIVMSG") >> " #" >> (_channel_group = +::boost::xpressive::range('a', 'z')) >> " :" >> (_message_group = +~::boost::xpressive::_ln) >> ::boost::xpressive::_ln)
            | (":tmi.twitch.tv " >> (_notice_group = "NOTICE") >> " * :" >> (_reason_group = +~::boost::xpressive::_ln) >> ::boost::xpressive::_ln)
            | (":tmi.twitch.tv " >> (_reconnect_group = "RECONNECT"))
        } {
    
    };

    ::boost::asio::thread_pool _context;
    ::boost::asio::ip::tcp::resolver _resolver;
    ::boost::asio::ssl::context _ssl_context;
    ::boost::beast::websocket::stream<::boost::asio::ssl::stream<::boost::beast::tcp_stream>> _stream;
    Handler& _handler;
    ::std::string _host;
    ::std::string _port;
    ::std::chrono::milliseconds _timeout;
    ::std::string _authority;
    ::std::string _username;
    ::std::string _access_token;
    ::boost::beast::flat_buffer _buffer;
    ::boost::xpressive::mark_tag _ping_group;
    ::boost::xpressive::mark_tag _username_group;
    ::boost::xpressive::mark_tag _privmsg_group;
    ::boost::xpressive::mark_tag _channel_group;
    ::boost::xpressive::mark_tag _message_group;
    ::boost::xpressive::mark_tag _notice_group;
    ::boost::xpressive::mark_tag _reason_group;
    ::boost::xpressive::mark_tag _reconnect_group;
    ::boost::xpressive::sregex _command_group;

    void on_init() {
        _authority.append(_host);
        _authority.append(":");
        _authority.append(_port);

        _ssl_context.set_verify_mode(::boost::asio::ssl::verify_none);
        if (!::SSL_set_tlsext_host_name(_stream.native_handle(), _host.c_str())) {
            throw ::boost::beast::system_error{
                static_cast<int>(::ERR_get_error()),
                ::boost::asio::error::get_ssl_category()
            };
        }
        _stream.set_verify_callback(::boost::asio::ssl::host_name_verification{ _host });
    };
    void on_connect() {
        ::boost::beast::get_lowest_layer(_stream).expires_after(_timeout);
        _resolver.async_resolve(_host, _port,
            ::boost::beast::bind_front_handler(&on_resolve, this));
    };
    void on_resolve(::boost::asio::ip::tcp::resolver::results_type results) {
        ::boost::beast::get_lowest_layer(_stream).async_connect(results,
            ::boost::beast::bind_front_handler(&on_transport_connect, this));
    };
    void on_transport_connect(::boost::asio::ip::tcp::resolver::results_type::endpoint_type) {
        _stream.next_layer().async_handshake(::boost::asio::ssl::stream_base::client,
            ::boost::beast::bind_front_handler(&on_ssl_handshake, this));
    };
    void on_ssl_handshake() {
        ::boost::beast::get_lowest_layer(_stream).expires_never();
        _stream.set_option(::boost::beast::websocket::stream_base::timeout::suggested(::boost::beast::role_type::client));
        _stream.async_handshake(_authority, "/",
            ::boost::beast::bind_front_handler(&on_websocket_handshake, this));
    };
    void on_websocket_handshake() {
        const char PASS[]{ "PASS oauth:" };
        ::boost::asio::mutable_buffer request{ _buffer.prepare(::std::size(PASS) - 1 + _access_token.size()) };
        ::std::memcpy(request.data(), reinterpret_cast<const void*>(PASS), ::std::size(PASS) - 1);
        ::std::memcpy(request.data() + ::std::size(PASS) - 1, reinterpret_cast<const void*>(_access_token.cdata()), _access_token.size());
        _stream.async_write(request,
            ::boost::beast::bind_front_handler(&on_pass, this));
    };
    void on_pass(size_t bytes_transferred) {
        ::boost::ignore_unused(bytes_transferred);
        const char NICK[]{ "NICK " };
        ::boost::asio::mutable_buffer request{ _buffer.prepare(::std::size(NICK) - 1 + _username.size()) };
        ::std::memcpy(request.data(), reinterpret_cast<const void*>(NICK), ::std::size(NICK) - 1);
        ::std::memcpy(request.data() + ::std::size(NICK) - 1, reinterpret_cast<const void*>(_username.cdata()), _username.size());
        _stream.async_write(request,
            ::boost::beast::bind_front_handler(&on_nick, this));
    };
    void on_nick(size_t bytes_transferred) {
        ::boost::ignore_unused(bytes_transferred);
        _stream.async_read(_buffer,
            ::boost::beast::bind_front_handler(&on_notice, this));
    };
    void on_notice(size_t bytes_transferred) {
        ::boost::xpressive::smatch what{};
        ::boost::xpressive::regex_match(::boost::asio::buffers_begin(_buffer), ::boost::asio::buffers_end(_buffer), what, _command_group);
        if (what[_notice_group]) {
            throw ::std::invalid_argument{ what[_reason_group] };
        }
        _buffer.consume(bytes_transferred);
        _stream.async_write(::boost::asio::buffer(_join),
            ::boost::beast::bind_front_handler(&on_join, this));
    };
    void on_join(size_t bytes_transferred) {
        ::boost::ignore_unused(bytes_transferred);
        _stream.async_read(_buffer,
            ::boost::beast::bind_front_handler(&on_read, this));
    };
    void on_read(size_t bytes_transferred) {
        bool pong{ true };
        bool reconnect{ false };
        for (::boost::xpressive::sregex_iterator current{ ::boost::asio::buffers_begin(_buffer), ::boost::asio::buffers_end(_buffer), _command_group }, end{}; !(current == end); ++current) {
            const ::boost::xpressive::smatch& what{ *current };
            if (what[_ping_group] && pong) {
                const char PONG[]{ "PONG :tmi.twitch.tv" };
                ::boost::asio::mutable_buffer request{ _buffer.prepare(::std::size(PONG) - 1) };
                ::std::memcpy(request.data(), reinterpret_cast<const void*>(PONG), ::std::size(PONG) - 1);
                _stream.async_write(request,
                    ::boost::beast::bind_front_handler(&on_pong, this));
                pong = false;
            } else if (what[_privmsg_group]) {
                _handler.on_message({
                    what[_username_group],
                    what[_message_group],
                    what[_channel_group]
                });
            } else if (what[_reconnect_group]) {
                reconnect = true;
            }
        }
        _buffer.consume(bytes_transferred);
        if (reconnect) {
            ::boost::asio::post(_context,
                ::boost::beast::bind_front_handler(&on_reconnect, this));
        } else {
            _stream.async_read(_buffer,
                ::boost::beast::bind_front_handler(&on_read, this));
        }
    };
    void on_pong(size_t bytes_transferred) {
        ::boost::ignore_unused(bytes_transferred);
    };
    void on_reconnect() {
        _stream.async_close(::boost::beast::websocket::close_code::normal,
            ::boost::beast::bind_front_handler(&on_connect, this));
    };
    void on_disconnect() {
        _stream.async_close(::boost::beast::websocket::close_code::normal,
            ::boost::beast::bind_front_handler(&on_close, this));
    };
    void on_close() {

    };
    void on_send(message_type message) {
        const char PRIVMSG[]{ "PRIVMSG #" };
        const char WHAT[]{ " :" };
        ::boost::asio::mutable_buffer request{ _buffer.prepare(::std::size(PRIVMSG) - 1 + message.channel.size() + ::std::size(WHAT) - 1 + message.content.size()) };
        ::std::memcpy(message.data(), reinterpret_cast<const void*>(PRIVMSG), ::std::size(PRIVMSG) - 1);
        ::std::memcpy(message.data() + ::std::size(PRIVMSG) - 1, reinterpret_cast<const void*>(message.channel.cdata()), message.channel.size());
        ::std::memcpy(message.data() + ::std::size(PRIVMSG) - 1 + message.channel.size(), reinterpret_cast<const void*>(WHAT), ::std::size(WHAT) - 1);
        ::std::memcpy(message.data() + ::std::size(PRIVMSG) - 1 + message.channel.size() + ::std::size(WHAT) - 1, reinterpret_cast<const void*>(message.content.cdata()), message.content.size());
        _stream.async_write(message,
            ::boost::beast::bind_front_handler(&on_write, this));
    };
    void on_write(size_t bytes_transferred) {
        ::boost::ignore_unused(bytes_transferred);
    };
};

template<typename Handler>
template<typename... Args>
irc<Handler>::irc(const ::std::string& address, ::std::chrono::milliseconds timeout, Args&& ...args)
    : Handler{ ::std::forward<Args>(args)... }
    , _p_channel{ new connection{ *this } } {
    ::boost::system::result<::boost::urls::url_view> result{ ::boost::urls::parse_uri(address) };
    if (!result.has_value()) {
        throw ::std::invalid_argument{ "invalid uri" };
    }
    ::boost::urls::url_view url{ result.value() };
    if (!(url.scheme() == "wss")) {
        throw ::std::invalid_argument{ "scheme is not supported" };
    }
    _p_channel->_host.append(url.host());
    _p_channel->_port.append(url.has_port()
        ? url.port()
        : "443");
    _p_channel->timeout = timeout;
    _p_channel->on_init();
};

template<typename Handler>
void irc<Handler>::connect(const ::std::string& username, const ::std::string& access_token) {
    _p_channel->_username.append(username);
    _p_channel->_access_token.append(access_token);

    ::boost::asio::post(_p_channel->_context,
        ::boost::beast::bind_front_handler(&channel::on_connect, _p_channel.get()));
};
template<typename Handler>
void irc<Handler>::disconnect() {
    ::boost::asio::post(_p_channel->_context,
        ::boost::beast::bind_front_handler(&channel::on_disconnect, _p_channel.get()));
};
template<typename Handler>
void irc<Handler>::send(const message_type& message) {
    ::boost::asio::post(_p_channel->_context,
        ::boost::beast::bind_front_handler(&channel::on_send, _p_channel.get(), message));
};

} // twitch
} // clients
} // chat
} // ai

#endif
