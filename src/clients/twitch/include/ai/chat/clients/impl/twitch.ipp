#ifndef AI_CHAT_CLIENTS_TWITCH_IPP
#define AI_CHAT_CLIENTS_TWITCH_IPP

#include <memory>
#include <queue>
#include <stdexcept>
#include <utility>

#include "boost/asio/buffer.hpp"
#include "boost/asio/signal_set.hpp"
#include "boost/asio/thread_pool.hpp"
#include "boost/asio/ssl.hpp"
#include "boost/beast.hpp"
#include "boost/beast/ssl.hpp"
#include "boost/url.hpp"
#include "boost/xpressive/xpressive.hpp"

#include "opentelemetry/metrics/provider.h"
#include "opentelemetry/trace/provider.h"

#include "eboost/beast/ensure_success.hpp"

#include "ai/chat/clients/twitch.hpp"

namespace ai {
namespace chat {
namespace clients {

template<typename Handler>
class twitch<Handler>::connection {
    friend twitch;

public:
    connection() = delete;
    connection(const connection&) = delete;
    connection(connection&&) = delete;

    ~connection() = default;

    connection& operator=(const connection&) = delete;
    connection& operator=(connection&&) = delete;

private:
    class rate_policy {
    public:
        rate_policy() = delete;
        rate_policy(const rate_policy&) = delete;
        rate_policy(rate_policy&&) = default;

        ~rate_policy() = default;

        rate_policy& operator=(const rate_policy&) = delete;
        rate_policy& operator=(rate_policy&&) = delete;

    private:
        friend ::boost::beast::rate_policy_access;
        friend connection;

        explicit rate_policy(connection& connection)
            : _connection{ connection } {

        };

        connection& _connection;

        size_t available_read_bytes() const {
            return ::std::numeric_limits<size_t>::max();
        };
        size_t available_write_bytes() const {
            auto now = ::std::chrono::steady_clock::now();
            return now.time_since_epoch() < _connection._next
                ? 0
                : ::std::numeric_limits<size_t>::max();
        };
        void transfer_read_bytes(size_t n) {
            _connection._p_bytes_rx->Add(n);
        };
        void transfer_write_bytes(size_t n) {
            _connection._p_bytes_tx->Add(n);
        };
        void on_timer() const {

        };
    };
    using limited_tcp_stream = ::boost::beast::basic_stream<::boost::asio::ip::tcp, ::boost::asio::any_io_executor, rate_policy>;

    explicit connection(size_t dop, twitch<Handler>& handler)
        : _context{ dop }
        , _resolver{ _context }
        , _ssl_context{ ::boost::asio::ssl::context::tlsv12_client }
        , _stream{ limited_tcp_stream{ rate_policy{ *this }, _context }, _ssl_context }
        , _p_tracer{
            ::opentelemetry::trace::Provider::GetTracerProvider()
                ->GetTracer("ai_chat_clients_twitch")
        }
        , _p_meter{
            ::opentelemetry::metrics::Provider::GetMeterProvider()
                ->GetMeter("ai_chat_clients_twitch")
        }
        , _p_bytes_tx{ _p_meter->CreateUInt64Counter("ai_chat_clients_twitch_bytes_tx") }
        , _p_bytes_rx{ _p_meter->CreateUInt64Counter("ai_chat_clients_twitch_bytes_rx") }
        , _signals{ _context }
        , _handler{ handler }
        , _host{}
        , _port{}
        , _timeout{}
        , _delay{}
        , _authority{}
        , _username{}
        , _access_token{}
        , _channel{}
        , _message{}
        , _q_write{}
        , _buffer{}
        , _next{}
        , _ping_group{ 1 }
        , _username_group{ 2 }
        , _privmsg_group{ 3 }
        , _channel_group{ 4 }
        , _command_group{ 5 }
        , _args_group{ 6 }
        , _message_group{ 7 }
        , _notice_group{ 8 }
        , _reason_group{ 9 }
        , _reconnect_group{ 10 }
        , _command_pattern{
            ((_ping_group = "PING") >> " :tmi.twitch.tv" >> ::boost::xpressive::_ln)
            | (':' >> (_username_group = +::boost::xpressive::range('a', 'z')) >> '!' >> _username_group >> '@' >> _username_group >> ".tmi.twitch.tv "
                >> (_privmsg_group = "PRIVMSG") >> " #" >> (_channel_group = +::boost::xpressive::range('a', 'z')) >> " :" >> (
                    ('!' >> (_command_group = +::boost::xpressive::range('a', 'z')) >> *(' ' >> (_args_group = +~::boost::xpressive::_ln)))
                    | (_message_group = +~::boost::xpressive::_ln)) >> ::boost::xpressive::_ln)
            | (":tmi.twitch.tv " >> (_notice_group = "NOTICE") >> " * :" >> (_reason_group = +~::boost::xpressive::_ln) >> ::boost::xpressive::_ln)
            | (":tmi.twitch.tv " >> (_reconnect_group = "RECONNECT"))
        } {
    
    };

    ::boost::asio::thread_pool _context;
    ::boost::asio::ip::tcp::resolver _resolver;
    ::boost::asio::ssl::context _ssl_context;
    ::boost::beast::websocket::stream<::boost::asio::ssl::stream<limited_tcp_stream>> _stream;
    ::opentelemetry::nostd::shared_ptr<::opentelemetry::trace::Tracer> _p_tracer;
    ::opentelemetry::nostd::shared_ptr<::opentelemetry::metrics::Meter> _p_meter;
    ::opentelemetry::nostd::unique_ptr<::opentelemetry::metrics::Counter<uint64_t>> _p_bytes_tx;
    ::opentelemetry::nostd::unique_ptr<::opentelemetry::metrics::Counter<uint64_t>> _p_bytes_rx;
    ::boost::asio::signal_set _signals;
    twitch<Handler>& _handler;
    ::std::string _host;
    ::std::string _port;
    ::std::chrono::milliseconds _timeout;
    ::std::chrono::milliseconds _delay;
    ::std::string _authority;
    ::std::string _username;
    ::std::string _access_token;
    ::std::string _channel;
    message _message;
    ::std::queue<::std::string> _q_write;
    ::boost::beast::flat_buffer _buffer;
    ::std::chrono::nanoseconds _next;
    ::boost::xpressive::mark_tag _ping_group;
    ::boost::xpressive::mark_tag _username_group;
    ::boost::xpressive::mark_tag _privmsg_group;
    ::boost::xpressive::mark_tag _channel_group;
    ::boost::xpressive::mark_tag _command_group;
    ::boost::xpressive::mark_tag _args_group;
    ::boost::xpressive::mark_tag _message_group;
    ::boost::xpressive::mark_tag _notice_group;
    ::boost::xpressive::mark_tag _reason_group;
    ::boost::xpressive::mark_tag _reconnect_group;
    ::boost::xpressive::cregex _command_pattern;

    void on_init() {
        _authority.append(_host);
        _authority.append(":");
        _authority.append(_port);

        _ssl_context.set_verify_mode(::boost::asio::ssl::verify_none);

        _signals.add(SIGINT);
        _signals.add(SIGTERM);
        _signals.async_wait([this](boost::system::error_code error_code, int) {
        ::eboost::beast::ensure_success(error_code);
        on_disconnect();

        }); // SIG
    };

    #define GUARD(error_code)\
    if (error_code == ::boost::beast::errc::operation_canceled) {\
        return;\
    }\
    ::eboost::beast::ensure_success(error_code)\


    void on_connect(::opentelemetry::nostd::shared_ptr<::opentelemetry::trace::Scope> p_scope = ::opentelemetry::nostd::shared_ptr<::opentelemetry::trace::Scope>{ nullptr }) {
        if (!p_scope) {
            p_scope = ::opentelemetry::nostd::shared_ptr<::opentelemetry::trace::Scope>{
                new ::opentelemetry::trace::Scope{ _p_tracer->StartSpan("on_connect") }
            };
        }
        if (!::SSL_set_tlsext_host_name(_stream.next_layer().native_handle(), _host.c_str())) {
            throw ::boost::beast::system_error{
                static_cast<int>(::ERR_get_error()),
                ::boost::asio::error::get_ssl_category()
            };
        }
        _stream.next_layer().set_verify_callback(::boost::asio::ssl::host_name_verification{ _host });

        ::boost::beast::get_lowest_layer(_stream).expires_after(_timeout);
        _resolver.async_resolve(_host, _port, [this, p_scope](::boost::beast::error_code error_code, ::boost::asio::ip::tcp::resolver::results_type results)->void {
        GUARD(error_code);
        ::boost::beast::get_lowest_layer(_stream).async_connect(results, [this, p_scope](::boost::beast::error_code error_code, ::boost::asio::ip::tcp::resolver::results_type::endpoint_type endpoint_type)->void {
        GUARD(error_code);
        _stream.next_layer().async_handshake(::boost::asio::ssl::stream_base::client, [this, p_scope](::boost::beast::error_code error_code)->void {
        GUARD(error_code);
        ::boost::beast::get_lowest_layer(_stream).expires_never();
        _stream.set_option(::boost::beast::websocket::stream_base::timeout::suggested(::boost::beast::role_type::client));
        _stream.async_handshake(_authority, "/", [this, p_scope](::boost::beast::error_code error_code)->void {
        GUARD(error_code);
        const char PASS[]{ "PASS oauth:" };
        ::boost::asio::mutable_buffer request{ _buffer.prepare(::std::size(PASS) - 1 + _access_token.size()) };
        ::std::memcpy(reinterpret_cast<char*>(request.data()), PASS, ::std::size(PASS) - 1);
        ::std::memcpy(reinterpret_cast<char*>(request.data()) + ::std::size(PASS) - 1, _access_token.data(), _access_token.size());
        _stream.async_write(request, [this, p_scope](::boost::beast::error_code error_code, size_t bytes_transferred)->void {
        GUARD(error_code);
        ::boost::ignore_unused(bytes_transferred);
        const char NICK[]{ "NICK " };
        ::boost::asio::mutable_buffer request{ _buffer.prepare(::std::size(NICK) - 1 + _username.size()) };
        ::std::memcpy(reinterpret_cast<char*>(request.data()), NICK, ::std::size(NICK) - 1);
        ::std::memcpy(reinterpret_cast<char*>(request.data()) + ::std::size(NICK) - 1, _username.data(), _username.size());
        _stream.async_write(request, [this, p_scope](::boost::beast::error_code error_code, size_t bytes_transferred)->void {
        GUARD(error_code);
        ::boost::ignore_unused(bytes_transferred);
        _stream.async_read(_buffer, [this, p_scope](::boost::beast::error_code error_code, size_t bytes_transferred)->void {
        GUARD(error_code);
        ::boost::asio::const_buffer response{ _buffer.cdata() };
        ::boost::xpressive::cmatch what{};
        ::boost::xpressive::regex_match(reinterpret_cast<const char*>(response.data()), reinterpret_cast<const char*>(response.data()) + bytes_transferred, what, _command_pattern);
        if (what[_notice_group]) {
            throw ::std::invalid_argument{ what[_reason_group] };
        }
        _buffer.consume(bytes_transferred);
        const char JOIN[]{ "JOIN #" };
        const char AND[]{ ",#" };
        ::boost::asio::mutable_buffer request{ _buffer.prepare(::std::size(JOIN) - 1 + _username.size() + (!_channel.empty() ? ::std::size(AND) - 1 + _channel.size() : 0)) };
        ::std::memcpy(reinterpret_cast<char*>(request.data()), JOIN, ::std::size(JOIN) - 1);
        ::std::memcpy(reinterpret_cast<char*>(request.data()) + ::std::size(JOIN) - 1, _username.data(), _username.size());
        if (!_channel.empty()) {
            ::std::memcpy(reinterpret_cast<char*>(request.data()) + ::std::size(JOIN) - 1 + _username.size(), AND, ::std::size(AND) - 1);
            ::std::memcpy(reinterpret_cast<char*>(request.data()) + ::std::size(JOIN) - 1 + _username.size() + ::std::size(AND) - 1, _channel.data(), _channel.size());
        }
        _stream.async_write(request, [this, p_scope](::boost::beast::error_code error_code, size_t bytes_transferred)->void {
        GUARD(error_code);
        ::boost::ignore_unused(bytes_transferred);
        on_read();

        }); // JOIN
        }); // NOTICE
        }); // NICK
        }); // PASS
        }); // WebSocket handshake
        }); // SSL handshake
        }); // TCP connect
        }); // resolve
    };
    void on_read() {
        _stream.async_read(_buffer, [this](::boost::beast::error_code error_code, size_t bytes_transferred)->void {
        GUARD(error_code);
        ::boost::asio::const_buffer response{ _buffer.cdata() };
        bool reconnect{ false };
        for (::boost::xpressive::cregex_iterator current{ reinterpret_cast<const char*>(response.data()), reinterpret_cast<const char*>(response.data()) + bytes_transferred, _command_pattern }, end{}; !(current == end); ++current) {
            const ::boost::xpressive::cmatch& what{ *current };
            if (what[_ping_group]) {
                ::opentelemetry::trace::Scope scope{ _p_tracer->StartSpan("on_pong") };
                on_push("PONG :tmi.twitch.tv");
            } else if (what[_privmsg_group]) {
                if (what[_command_group]) {
                    ::opentelemetry::trace::Scope scope{ _p_tracer->StartSpan("on_command") };
                    _handler.on_command({
                        what[_username_group],
                        what[_command_group],
                        what[_args_group],
                        what[_channel_group]
                    });
                } else {
                    ::opentelemetry::trace::Scope scope{ _p_tracer->StartSpan("on_message") };
                    _handler.on_message({
                        what[_username_group],
                        what[_message_group],
                        what[_channel_group]
                    });
                }
            } else if (what[_reconnect_group]) {
                reconnect = true;
            }
        }
        _buffer.consume(bytes_transferred);
        if (reconnect) {
            ::opentelemetry::nostd::shared_ptr<::opentelemetry::trace::Scope> p_scope{
                new ::opentelemetry::trace::Scope{ _p_tracer->StartSpan("on_reconnect") }
            };
            ::boost::asio::post(_context, [this, p_scope]()->void {
            _stream.async_close(::boost::beast::websocket::close_code::normal, [this, p_scope](::boost::beast::error_code error_code)->void {
            GUARD(error_code);
            _stream.~stream();
            new(&_stream) ::boost::beast::websocket::stream<::boost::asio::ssl::stream<::boost::beast::tcp_stream>>{ _context, _ssl_context };
            on_connect(p_scope);

            }); // close
            }); // post
        } else {
            on_read();
        }

        }); // read
    };
    void on_push(::std::string&& request) {
        _q_write.push(::std::move(request));
        if (1 < _q_write.size()) {
            return;
        }
        on_write();
    };
    void on_write(::opentelemetry::nostd::shared_ptr<::opentelemetry::trace::Scope> p_scope = ::opentelemetry::nostd::shared_ptr<::opentelemetry::trace::Scope>{ nullptr }) {
        if (!p_scope) {
            p_scope = ::opentelemetry::nostd::shared_ptr<::opentelemetry::trace::Scope>{
                new ::opentelemetry::trace::Scope{ _p_tracer->StartSpan("on_write") }
            };
        }
        _stream.async_write(::boost::asio::buffer(_q_write.front()), [this, p_scope](::boost::beast::error_code error_code, size_t bytes_transferred)->void {
        GUARD(error_code);
        ::boost::ignore_unused(bytes_transferred);
        auto now = ::std::chrono::steady_clock::now();
        _next = now.time_since_epoch() + _delay;
        _q_write.pop();
        if (_q_write.empty()) {
            return;
        }
        on_write(p_scope);

        }); // write
    };
    void on_disconnect() {
        ::opentelemetry::nostd::shared_ptr<::opentelemetry::trace::Scope> p_scope{
            new ::opentelemetry::trace::Scope{ _p_tracer->StartSpan("on_disconnect") }
        };
        _stream.async_close(::boost::beast::websocket::close_code::normal, [this, p_scope](::boost::beast::error_code error_code)->void {
        _context.stop();

        }); // close
    };
    void on_send() {
        on_push("PRIVMSG #" + _message.channel + " :" + _message.content);
    };
    void on_join() {
        on_push("JOIN #" + _channel);
    };
    void on_leave() {
        on_push("PART #" + _channel);
    };
};

template<typename Handler>
twitch<Handler>::twitch(const ::std::string& address, ::std::chrono::milliseconds timeout,
    ::std::chrono::milliseconds delay, size_t dop)
    : _p_channel{ new connection{ dop, *this } } {
    ::boost::system::result<::boost::urls::url_view> result{ ::boost::urls::parse_uri(address) };
    if (!result.has_value()) {
        throw ::std::invalid_argument{ "invalid uri" };
    }
    ::boost::urls::url_view url{ result.value() };
    if (!(url.scheme() == "wss")) {
        throw ::std::invalid_argument{ "scheme is not supported" };
    }
    _p_channel->_host = url.host();
    _p_channel->_port = url.has_port()
        ? url.port()
        : "443";
    _p_channel->_timeout = timeout;
    _p_channel->_delay = delay;
    _p_channel->on_init();
};

template<typename Handler>
void twitch<Handler>::connect(const ::std::string& username, const ::std::string& access_token) {
    ::opentelemetry::trace::Scope scope{ _p_channel->_p_tracer->StartSpan("connect") };
    _p_channel->_username = username;
    _p_channel->_access_token = access_token;
    _p_channel->on_connect();
};
template<typename Handler>
void twitch<Handler>::disconnect() {
    ::opentelemetry::trace::Scope scope{ _p_channel->_p_tracer->StartSpan("disconnect") };
    ::boost::asio::post(_p_channel->_context, [this]()->void {
    _p_channel->on_disconnect();

    }); // post
    _p_channel->_context.wait();
};
template<typename Handler>
void twitch<Handler>::send(const message& message) {
    ::opentelemetry::nostd::shared_ptr<::opentelemetry::trace::Scope> p_scope{
        new ::opentelemetry::trace::Scope{ _p_channel->_p_tracer->StartSpan("send") }
    };
    ::boost::asio::post(_p_channel->_context, [this, message, p_scope]()->void {
    _p_channel->_message = ::std::move(message);
    _p_channel->on_send();

    }); // post
};

template<typename Handler>
void twitch<Handler>::attach() {
    _p_channel->_context.attach();
};
template<typename Handler>
void twitch<Handler>::join(const ::std::string& channel) {
    ::opentelemetry::nostd::shared_ptr<::opentelemetry::trace::Scope> p_scope{
        new ::opentelemetry::trace::Scope{ _p_channel->_p_tracer->StartSpan("join") }
    };
    ::boost::asio::post(_p_channel->_context, [this, channel, p_scope]()->void {
    _p_channel->_channel = ::std::move(channel);
    _p_channel->on_join();

    }); // post
};
template<typename Handler>
void twitch<Handler>::leave() {
    ::opentelemetry::nostd::shared_ptr<::opentelemetry::trace::Scope> p_scope{
        new ::opentelemetry::trace::Scope{ _p_channel->_p_tracer->StartSpan("leave") }
    };
    ::boost::asio::post(_p_channel->_context, [this, channel, p_scope]()->void {
    _p_channel->on_leave();
    _p_channel->_channel.clear();

    }); // post
};

template<typename Handler>
void twitch<Handler>::on_message(const message& message) const {
    static_cast<const Handler*>(this)->on_message(message);
};
template<typename Handler>
void twitch<Handler>::on_command(const command& command) const {
    static_cast<const Handler*>(this)->on_command(command);
};

} // clients
} // chat
} // ai

#endif
