#ifndef AI_CHAT_CLIENTS_TWITCH_IPP
#define AI_CHAT_CLIENTS_TWITCH_IPP

#include <memory>
#include <queue>
#include <stdexcept>
#include <thread>
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
#include "eboost/beast/metered_rate_policy.hpp"

#include "ai/chat/clients/twitch.hpp"

namespace ai {
namespace chat {
namespace clients {

template<typename Handler>
class twitch<Handler>::connection {
public:
    connection() = delete;
    connection(const connection&) = delete;
    connection(connection&&) = delete;

    ~connection() = default;

    connection& operator=(const connection&) = delete;
    connection& operator=(connection&&) = delete;

private:
    friend ::eboost::beast::metered_rate_policy<connection>;
    friend twitch;

    explicit connection(size_t dop, twitch<Handler>& handler)
        : _io_context{}
        , _io_run_context{ 1 }
        , _h_context{ dop }
        , _resolver{ _io_context }
        , _ssl_context{ ::boost::asio::ssl::context::tlsv12_client }
        , _stream{ ::eboost::beast::metered_tcp_stream<connection>{ ::eboost::beast::metered_rate_policy<connection>{ *this }, _io_context }, _ssl_context }
        , _signals{ _io_context }
        , _handler{ handler }
        , _tracer{
            ::opentelemetry::trace::Provider::GetTracerProvider()
                ->GetTracer("ai_chat_clients_twitch")
        }
        , _meter{
            ::opentelemetry::metrics::Provider::GetMeterProvider()
                ->GetMeter("ai_chat_clients_twitch")
        }
        , _m_network{ _meter->CreateUInt64Counter("ai_chat_clients_twitch_network") }
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

    ::boost::asio::io_context _io_context;
    ::boost::asio::thread_pool _io_run_context;
    ::boost::asio::thread_pool _h_context;
    ::boost::asio::ip::tcp::resolver _resolver;
    ::boost::asio::ssl::context _ssl_context;
    ::boost::beast::websocket::stream<::boost::asio::ssl::stream<::eboost::beast::metered_tcp_stream<connection>>> _stream;
    ::boost::asio::signal_set _signals;
    twitch<Handler>& _handler;
    ::opentelemetry::nostd::shared_ptr<::opentelemetry::trace::Tracer> _tracer;
    ::opentelemetry::nostd::shared_ptr<::opentelemetry::metrics::Meter> _meter;
    ::opentelemetry::nostd::unique_ptr<::opentelemetry::metrics::Counter<uint64_t>> _m_network;
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

    void bytes_rx(size_t n) {
        _m_network->Add(n,
        {
            {"type", "rx"}
        });
    };
    void bytes_tx(size_t n) {
        _m_network->Add(n,
        {
            {"type", "tx"}
        });
    };

    void on_init() {
        _authority.append(_host);
        _authority.append(":");
        _authority.append(_port);
        _ssl_context.set_verify_mode(::boost::asio::ssl::verify_none);
        _signals.add(SIGINT);
        _signals.add(SIGTERM);
        _signals.async_wait([this](boost::system::error_code error_code, int) {
        ::eboost::beast::ensure_success(error_code);
        ::opentelemetry::nostd::shared_ptr<::opentelemetry::trace::Span> span{
            _tracer->StartSpan("on_signal")
        };
        on_disconnect(span);

        }); // SIG
        ::boost::asio::post(_io_run_context, [this]()->void {
        _io_context.run();

        }); // post
    };

    void on_connect(::opentelemetry::nostd::shared_ptr<::opentelemetry::trace::Span> root) {
        ::opentelemetry::nostd::shared_ptr<::opentelemetry::trace::Span> span{
            _tracer->StartSpan("on_connect", ::opentelemetry::trace::StartSpanOptions
            {
                {}, {},
                root->GetContext()
            })
        };
        if (!::SSL_set_tlsext_host_name(_stream.next_layer().native_handle(), _host.c_str())) {
            throw ::boost::beast::system_error{
                static_cast<int>(::ERR_get_error()),
                ::boost::asio::error::get_ssl_category()
            };
        }
        _stream.next_layer().set_verify_callback(::boost::asio::ssl::host_name_verification{ _host });
        ::boost::beast::get_lowest_layer(_stream).expires_after(_timeout);
        ::opentelemetry::nostd::shared_ptr<::opentelemetry::trace::Span> operation{
            _tracer->StartSpan("on_dns_resolve", ::opentelemetry::trace::StartSpanOptions
            {
                {}, {},
                span->GetContext()
            })
        };
        _resolver.async_resolve(_host, _port, [this, span, operation](::boost::beast::error_code error_code, ::boost::asio::ip::tcp::resolver::results_type results) mutable ->void {
        if (error_code == ::boost::beast::errc::operation_canceled) {
            return;
        }
        ::eboost::beast::ensure_success(error_code);
        operation = _tracer->StartSpan("on_tcp_connect", ::opentelemetry::trace::StartSpanOptions
        {
            {}, {},
            span->GetContext()
        });
        ::boost::beast::get_lowest_layer(_stream).async_connect(results, [this, span, operation](::boost::beast::error_code error_code, ::boost::asio::ip::tcp::resolver::results_type::endpoint_type endpoint_type) mutable ->void {
        if (error_code == ::boost::beast::errc::operation_canceled) {
            return;
        }
        ::eboost::beast::ensure_success(error_code);
        operation = _tracer->StartSpan("on_ssl_handshake", ::opentelemetry::trace::StartSpanOptions
        {
            {}, {},
            span->GetContext()
        });
        _stream.next_layer().async_handshake(::boost::asio::ssl::stream_base::client, [this, span, operation](::boost::beast::error_code error_code) mutable ->void {
        if (error_code == ::boost::beast::errc::operation_canceled) {
            return;
        }
        ::eboost::beast::ensure_success(error_code);
        operation = nullptr;
        ::boost::beast::get_lowest_layer(_stream).expires_never();
        _stream.set_option(::boost::beast::websocket::stream_base::timeout::suggested(::boost::beast::role_type::client));
        operation = _tracer->StartSpan("on_ws_handshake", ::opentelemetry::trace::StartSpanOptions
        {
            {}, {},
            span->GetContext()
        });
        _stream.async_handshake(_authority, "/", [this, span, operation](::boost::beast::error_code error_code) mutable ->void {
        if (error_code == ::boost::beast::errc::operation_canceled) {
            return;
        }
        ::eboost::beast::ensure_success(error_code);
        operation = _tracer->StartSpan("on_pass", ::opentelemetry::trace::StartSpanOptions
        {
            {}, {},
            span->GetContext()
        });
        const char PASS[]{ "PASS oauth:" };
        ::boost::asio::mutable_buffer request{ _buffer.prepare(::std::size(PASS) - 1 + _access_token.size()) };
        ::std::memcpy(reinterpret_cast<char*>(request.data()), PASS, ::std::size(PASS) - 1);
        ::std::memcpy(reinterpret_cast<char*>(request.data()) + ::std::size(PASS) - 1, _access_token.data(), _access_token.size());
        _stream.async_write(request, [this, span, operation](::boost::beast::error_code error_code, size_t bytes_transferred) mutable ->void {
        if (error_code == ::boost::beast::errc::operation_canceled) {
            return;
        }
        ::eboost::beast::ensure_success(error_code);
        ::boost::ignore_unused(bytes_transferred);
        operation = _tracer->StartSpan("on_nick", ::opentelemetry::trace::StartSpanOptions
        {
            {}, {},
            span->GetContext()
        });
        const char NICK[]{ "NICK " };
        ::boost::asio::mutable_buffer request{ _buffer.prepare(::std::size(NICK) - 1 + _username.size()) };
        ::std::memcpy(reinterpret_cast<char*>(request.data()), NICK, ::std::size(NICK) - 1);
        ::std::memcpy(reinterpret_cast<char*>(request.data()) + ::std::size(NICK) - 1, _username.data(), _username.size());
        _stream.async_write(request, [this, span, operation](::boost::beast::error_code error_code, size_t bytes_transferred) mutable ->void {
        if (error_code == ::boost::beast::errc::operation_canceled) {
            return;
        }
        ::eboost::beast::ensure_success(error_code);
        ::boost::ignore_unused(bytes_transferred);
        operation = _tracer->StartSpan("on_notice", ::opentelemetry::trace::StartSpanOptions
        {
            {}, {},
            span->GetContext()
        });
        _stream.async_read(_buffer, [this, span, operation](::boost::beast::error_code error_code, size_t bytes_transferred) mutable ->void {
        if (error_code == ::boost::beast::errc::operation_canceled) {
            return;
        }
        ::eboost::beast::ensure_success(error_code);
        ::boost::asio::const_buffer response{ _buffer.cdata() };
        ::boost::xpressive::cmatch what{};
        ::boost::xpressive::regex_match(reinterpret_cast<const char*>(response.data()), reinterpret_cast<const char*>(response.data()) + bytes_transferred, what, _command_pattern);
        if (what[_notice_group]) {
            throw ::std::invalid_argument{ what[_reason_group] };
        }
        _buffer.consume(bytes_transferred);
        operation = _tracer->StartSpan("on_join", ::opentelemetry::trace::StartSpanOptions
        {
            {}, {},
            span->GetContext()
        });
        const char JOIN[]{ "JOIN #" };
        const char AND[]{ ",#" };
        ::boost::asio::mutable_buffer request{ _buffer.prepare(::std::size(JOIN) - 1 + _username.size() + (!_channel.empty() ? ::std::size(AND) - 1 + _channel.size() : 0)) };
        ::std::memcpy(reinterpret_cast<char*>(request.data()), JOIN, ::std::size(JOIN) - 1);
        ::std::memcpy(reinterpret_cast<char*>(request.data()) + ::std::size(JOIN) - 1, _username.data(), _username.size());
        if (!_channel.empty()) {
            ::std::memcpy(reinterpret_cast<char*>(request.data()) + ::std::size(JOIN) - 1 + _username.size(), AND, ::std::size(AND) - 1);
            ::std::memcpy(reinterpret_cast<char*>(request.data()) + ::std::size(JOIN) - 1 + _username.size() + ::std::size(AND) - 1, _channel.data(), _channel.size());
        }
        _stream.async_write(request, [this, span, operation](::boost::beast::error_code error_code, size_t bytes_transferred) mutable ->void {
        if (error_code == ::boost::beast::errc::operation_canceled) {
            return;
        }
        ::eboost::beast::ensure_success(error_code);
        ::boost::ignore_unused(bytes_transferred);
        operation = nullptr;
        span = nullptr;
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
        ::opentelemetry::nostd::shared_ptr<::opentelemetry::trace::Span> operation{
            _tracer->StartSpan("on_read")
        };
        _stream.async_read(_buffer, [this, operation](::boost::beast::error_code error_code, size_t bytes_transferred) mutable ->void {
        if (error_code == ::boost::beast::errc::operation_canceled) {
            return;
        }
        ::eboost::beast::ensure_success(error_code);
        ::boost::asio::const_buffer response{ _buffer.cdata() };
        bool reconnect{ false };
        for (::boost::xpressive::cregex_iterator current{ reinterpret_cast<const char*>(response.data()), reinterpret_cast<const char*>(response.data()) + bytes_transferred, _command_pattern }, end{}; !(current == end); ++current) {
            const ::boost::xpressive::cmatch& what{ *current };
            if (what[_ping_group]) {
                ::opentelemetry::nostd::shared_ptr<::opentelemetry::trace::Span> span{
                    _tracer->StartSpan("on_pong", ::opentelemetry::trace::StartSpanOptions
                    {
                        {}, {},
                        operation->GetContext()
                    })
                };
                on_push("PONG :tmi.twitch.tv", span);
            } else if (what[_privmsg_group]) {
                if (what[_command_group]) {
                    ::opentelemetry::nostd::shared_ptr<::opentelemetry::trace::Span> span{
                        _tracer->StartSpan("command", ::opentelemetry::trace::StartSpanOptions
                        {
                            {}, {},
                            operation->GetContext()
                        })
                    };
                    ::boost::asio::post(_h_context, [this, command = command{
                        what[_username_group],
                        what[_command_group],
                        what[_args_group],
                        what[_channel_group]
                    }, span]()->void {
                    ::opentelemetry::trace::Scope scope{
                        _tracer->StartSpan("on_command", ::opentelemetry::trace::StartSpanOptions
                        {
                            {}, {},
                            span->GetContext()
                        })
                    };
                    _handler.on_command(command);

                    }); // post
                } else {
                    ::opentelemetry::nostd::shared_ptr<::opentelemetry::trace::Span> span{
                        _tracer->StartSpan("message", ::opentelemetry::trace::StartSpanOptions
                        {
                            {}, {},
                            operation->GetContext()
                        })
                    };
                    ::boost::asio::post(_h_context, [this, message = message{
                        what[_username_group],
                        what[_message_group],
                        what[_channel_group]
                    }, span]()->void {
                    ::opentelemetry::trace::Scope scope{
                        _tracer->StartSpan("on_message", ::opentelemetry::trace::StartSpanOptions
                        {
                            {}, {},
                            span->GetContext()
                        })
                    };
                    _handler.on_message(message);

                    }); // post
                }
            } else if (what[_reconnect_group]) {
                reconnect = true;
            }
        }
        _buffer.consume(bytes_transferred);
        if (reconnect) {
            ::opentelemetry::nostd::shared_ptr<::opentelemetry::trace::Span> span{
                _tracer->StartSpan("reconnect", ::opentelemetry::trace::StartSpanOptions
                {
                    {}, {},
                    operation->GetContext()
                })
            };
            operation = _tracer->StartSpan("on_close", ::opentelemetry::trace::StartSpanOptions
            {
                {}, {},
                span->GetContext()
            });
            _stream.async_close(::boost::beast::websocket::close_code::normal, [this, span, operation](::boost::beast::error_code error_code) mutable ->void {
            if (error_code == ::boost::beast::errc::operation_canceled) {
                return;
            }
            ::eboost::beast::ensure_success(error_code);
            operation = nullptr;
            _stream.~stream();
            new(&_stream) ::boost::beast::websocket::stream<::boost::asio::ssl::stream<::eboost::beast::metered_tcp_stream<connection>>>{
                ::eboost::beast::metered_tcp_stream<connection>{ ::eboost::beast::metered_rate_policy<connection>{ *this }, _io_context },
                _ssl_context
            };
            on_connect(span);

            }); // close
        } else {
            operation = nullptr;
            on_read();
        }

        }); // read
    };
    void on_push(::std::string&& request,
        ::opentelemetry::nostd::shared_ptr<::opentelemetry::trace::Span> root) {
        _q_write.push(::std::move(request));
        if (1 < _q_write.size()) {
            return;
        }
        on_write(root);
    };
    void on_write(::opentelemetry::nostd::shared_ptr<::opentelemetry::trace::Span> root) {
        auto now = ::std::chrono::steady_clock::now();
        if (now.time_since_epoch() < _next) {
            ::std::this_thread::sleep_for(_next - now.time_since_epoch());
        }
        ::opentelemetry::nostd::shared_ptr<::opentelemetry::trace::Span> operation{
            _tracer->StartSpan("on_write", ::opentelemetry::trace::StartSpanOptions
            {
                {}, {},
                root->GetContext()
            })
        };
        _stream.async_write(::boost::asio::buffer(_q_write.front()), [this, root, operation](::boost::beast::error_code error_code, size_t bytes_transferred) mutable ->void {
        if (error_code == ::boost::beast::errc::operation_canceled) {
            return;
        }
        ::eboost::beast::ensure_success(error_code);
        ::boost::ignore_unused(bytes_transferred);
        operation = nullptr;
        auto now = ::std::chrono::steady_clock::now();
        _next = now.time_since_epoch() + _delay;
        _q_write.pop();
        if (_q_write.empty()) {
            return;
        }
        on_write(root);

        }); // write
    };
    void on_disconnect(::opentelemetry::nostd::shared_ptr<::opentelemetry::trace::Span> root) {
        ::opentelemetry::nostd::shared_ptr<::opentelemetry::trace::Span> span{
            _tracer->StartSpan("on_disconnect", ::opentelemetry::trace::StartSpanOptions
            {
                {}, {},
                root->GetContext()
            })
        };
        ::opentelemetry::nostd::shared_ptr<::opentelemetry::trace::Span> operation{
            _tracer->StartSpan("on_close", ::opentelemetry::trace::StartSpanOptions
            {
                {}, {},
                span->GetContext()
            })
        };
        _stream.async_close(::boost::beast::websocket::close_code::normal, [this, span, operation](::boost::beast::error_code error_code) mutable ->void {
        operation = nullptr;
        _io_context.stop();
        _io_run_context.stop();
        _h_context.stop();

        }); // close
    };
    void on_send(::opentelemetry::nostd::shared_ptr<::opentelemetry::trace::Span> root) {
        ::opentelemetry::nostd::shared_ptr<::opentelemetry::trace::Span> span{
            _tracer->StartSpan("on_send", ::opentelemetry::trace::StartSpanOptions
            {
                {}, {},
                root->GetContext()
            })
        };
        on_push("PRIVMSG #" + _message.channel + " :" + _message.content,
            span);
    };
    void on_join(::opentelemetry::nostd::shared_ptr<::opentelemetry::trace::Span> root) {
        ::opentelemetry::nostd::shared_ptr<::opentelemetry::trace::Span> span{
            _tracer->StartSpan("on_join", ::opentelemetry::trace::StartSpanOptions
            {
                {}, {},
                root->GetContext()
            })
        };
        on_push("JOIN #" + _channel,
            span);
    };
    void on_leave(::opentelemetry::nostd::shared_ptr<::opentelemetry::trace::Span> root) {
        ::opentelemetry::nostd::shared_ptr<::opentelemetry::trace::Span> span{
            _tracer->StartSpan("on_leave", ::opentelemetry::trace::StartSpanOptions
            {
                {}, {},
                root->GetContext()
            })
        };
        on_push("PART #" + _channel,
            span);
    };
};

template<typename Handler>
twitch<Handler>::twitch(const ::std::string& address, ::std::chrono::milliseconds timeout,
    ::std::chrono::milliseconds delay, size_t dop)
    : _channel{ new connection{ dop, *this } } {
    ::boost::system::result<::boost::urls::url_view> result{ ::boost::urls::parse_uri(address) };
    if (!result.has_value()) {
        throw ::std::invalid_argument{ "invalid uri" };
    }
    ::boost::urls::url_view url{ result.value() };
    if (!(url.scheme() == "wss")) {
        throw ::std::invalid_argument{ "scheme is not supported" };
    }
    _channel->_host = url.host();
    _channel->_port = url.has_port()
        ? url.port()
        : "443";
    _channel->_timeout = timeout;
    _channel->_delay = delay;
    _channel->on_init();
};

template<typename Handler>
void twitch<Handler>::connect(const ::std::string& username, const ::std::string& access_token) {
    ::opentelemetry::nostd::shared_ptr<::opentelemetry::trace::Span> span{
        _channel->_tracer->StartSpan("connect")
    };
    ::boost::asio::post(_channel->_io_context, [this, username, access_token, span]()->void {
    _channel->_username = ::std::move(username);
    _channel->_access_token = ::std::move(access_token);
    _channel->on_connect(span);

    }); // post
};
template<typename Handler>
void twitch<Handler>::disconnect() {
    ::opentelemetry::nostd::shared_ptr<::opentelemetry::trace::Span> span{
        _channel->_tracer->StartSpan("disconnect")
    };
    ::boost::asio::post(_channel->_io_context, [this, span]()->void {
    _channel->on_disconnect(span);

    }); // post
    _channel->_io_context.wait();
};
template<typename Handler>
void twitch<Handler>::send(const message& message) {
    ::opentelemetry::nostd::shared_ptr<::opentelemetry::trace::Span> span{
        _channel->_tracer->StartSpan("send")
    };
    ::boost::asio::post(_channel->_io_context, [this, message, span]()->void {
    _channel->_message = ::std::move(message);
    _channel->on_send(span);

    }); // post
};

template<typename Handler>
void twitch<Handler>::attach() {
    _channel->_h_context.attach();
};
template<typename Handler>
void twitch<Handler>::join(const ::std::string& channel) {
    ::opentelemetry::nostd::shared_ptr<::opentelemetry::trace::Span> span{
        _channel->_tracer->StartSpan("join")
    };
    ::boost::asio::post(_channel->_io_context, [this, channel, span]()->void {
    _channel->_channel = ::std::move(channel);
    _channel->on_join(span);

    }); // post
};
template<typename Handler>
void twitch<Handler>::leave() {
    ::opentelemetry::nostd::shared_ptr<::opentelemetry::trace::Span> span{
        _channel->_tracer->StartSpan("leave")
    };
    ::boost::asio::post(_channel->_io_context, [this, channel, span]()->void {
    _channel->on_leave(span);
    _channel->_channel.clear();

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
