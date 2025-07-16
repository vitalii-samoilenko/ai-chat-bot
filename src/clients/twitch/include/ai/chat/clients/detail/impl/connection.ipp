#ifndef AI_CHAT_CLIENTS_DETAIL_CONNECTION_IPP
#define AI_CHAT_CLIENTS_DETAIL_CONNECTION_IPP

#include <thread>
#include <utility>

namespace ai {
namespace chat {
namespace clients {
namespace detail {

template<typename Handler>
connection<Handler>::connection(size_t dop, twitch<Handler> &handler)
    : _io_context{}
    , _io_run_context{ 1 }
    , _h_context{ dop }
    , _resolver{ _io_context }
    , _ssl_context{ ::boost::asio::ssl::context::tlsv12_client }
    , _stream{ ::eboost::beast::metered_tcp_stream<connection>{ ::eboost::beast::metered_rate_policy<connection>{ *this }, _io_context }, _ssl_context }
    , _signals{ _io_context }
    , _handler{ handler }
    , _logger{
        ::opentelemetry::logs::Provider::GetLoggerProvider()
            ->GetLogger("ai_chat_clients_twitch")
    }
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
    , _path{}
    , _timeout{}
    , _delay{}
    , _authority{}
    , _username{}
    , _access_token{}
    , _channel{}
    , _message_content{}
    , _message_channel{}
    , _q_write{}
    , _buffer{}
    , _next{}
    , _re_notice{ R"(:tmi\.twitch\.tv NOTICE \* :(?<notice>[^\r\n]+)[\r\n]*)" }
    , _re_line{ R"((?<line>[^\r\n]+))" }
    , _re_command_wargs{ R"(:(?<username>[a-z]+)![a-z]+@[a-z]+\.tmi\.twitch\.tv PRIVMSG #(?<channel>[a-z]+) :!(?<command>[a-z]+) (?<args>.+))" }
    , _re_command{ R"(:(?<username>[a-z]+)![a-z]+@[a-z]+\.tmi\.twitch\.tv PRIVMSG #(?<channel>[a-z]+) :!(?<command>[a-z]+))" }
    , _re_message{ R"(:(?<username>[a-z]+)![a-z]+@[a-z]+\.tmi\.twitch\.tv PRIVMSG #(?<channel>[a-z]+) :(?<message>.+))" }
    , _re_ping{ R"(PING (?<ping>:tmi\.twitch\.tv))" }
    , _re_reconnect{ R"((?<reconnect>:tmi\.twitch\.tv) RECONNECT)" } {

};

template<typename Handler>
void connection<Handler>::bytes_rx(size_t n) {
    _m_network->Add(n, {
        {"type", "rx"}
    });
};
template<typename Handler>
void connection<Handler>::bytes_tx(size_t n) {
    _m_network->Add(n, {
        {"type", "tx"}
    });
};

template<typename Handler>
void connection<Handler>::on_init() {
    _authority += _host;
    _authority += ":";
    _authority += _port;
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
template<typename Handler>
void connection<Handler>::on_connect(
    ::opentelemetry::nostd::shared_ptr<::opentelemetry::trace::Span> root) {
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
    _stream.async_handshake(_authority, _path, [this, span, operation](::boost::beast::error_code error_code) mutable ->void {
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
    ::std::memcpy(reinterpret_cast<char *>(request.data()), PASS, ::std::size(PASS) - 1);
    ::std::memcpy(reinterpret_cast<char *>(request.data()) + ::std::size(PASS) - 1, _access_token.data(), _access_token.size());
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
    ::std::memcpy(reinterpret_cast<char *>(request.data()), NICK, ::std::size(NICK) - 1);
    ::std::memcpy(reinterpret_cast<char *>(request.data()) + ::std::size(NICK) - 1, _username.data(), _username.size());
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
    _logger->Info(::opentelemetry::nostd::string_view{ reinterpret_cast<const char *>(response.data()), bytes_transferred }, operation->GetContext());
    {
        ::std::string_view notice{};
        ::std::string_view cursor{ reinterpret_cast<const char *>(response.data()), bytes_transferred };
        if (::RE2::FullMatch(cursor, _re_notice,
                &notice)) {
            throw ::std::invalid_argument{ ::std::string{ notice } };
        }
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
    ::std::memcpy(reinterpret_cast<char *>(request.data()), JOIN, ::std::size(JOIN) - 1);
    ::std::memcpy(reinterpret_cast<char *>(request.data()) + ::std::size(JOIN) - 1, _username.data(), _username.size());
    if (!_channel.empty()) {
        ::std::memcpy(reinterpret_cast<char *>(request.data()) + ::std::size(JOIN) - 1 + _username.size(), AND, ::std::size(AND) - 1);
        ::std::memcpy(reinterpret_cast<char *>(request.data()) + ::std::size(JOIN) - 1 + _username.size() + ::std::size(AND) - 1, _channel.data(), _channel.size());
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
template<typename Handler>
void connection<Handler>::on_read() {
    ::opentelemetry::nostd::shared_ptr<::opentelemetry::trace::Span> operation{
        _tracer->StartSpan("on_read")
    };
    _stream.async_read(_buffer, [this, operation](::boost::beast::error_code error_code, size_t bytes_transferred) mutable ->void {
    if (error_code == ::boost::beast::errc::operation_canceled) {
        return;
    }
    ::eboost::beast::ensure_success(error_code);
    ::boost::asio::const_buffer response{ _buffer.cdata() };
    _logger->Info(::opentelemetry::nostd::string_view{ reinterpret_cast<const char *>(response.data()), bytes_transferred }, operation->GetContext());
    bool pong{ false };
    bool disconnect{ false };
    {
        ::std::string_view line{};
        ::std::string_view cursor{ reinterpret_cast<const char *>(response.data()), bytes_transferred };
        while (::RE2::FindAndConsume(&cursor, _re_line,
                &line)) {
            ::std::string_view arg1{};
            ::std::string_view arg2{};
            ::std::string_view arg3{};
            ::std::string_view arg4{};
            if (::RE2::FullMatch(line, _re_command_wargs,
                    &arg1, &arg2, &arg3, &arg4)
                || ::RE2::FullMatch(line, _re_command,
                    &arg1, &arg2, &arg3)) {
                ::opentelemetry::nostd::shared_ptr<::opentelemetry::trace::Span> span{
                    _tracer->StartSpan("command", ::opentelemetry::trace::StartSpanOptions
                    {
                        {}, {},
                        operation->GetContext()
                    })
                };
                ::boost::asio::post(_h_context, [this,
                    username = ::std::string{ arg1 },
                    channel = ::std::string{ arg2 },
                    name = ::std::string{ arg3 },
                    args = ::std::string{ arg4 },
                    span]()->void {
                ::opentelemetry::trace::Scope scope{
                    _tracer->StartSpan("on_command", ::opentelemetry::trace::StartSpanOptions
                    {
                        {}, {},
                        span->GetContext()
                    })
                };
                _handler.on_command(command{
                    username,
                    name, args,
                    channel
                });

                }); // post
            } else if (::RE2::FullMatch(line, _re_message,
                    &arg1, &arg2, &arg3)) {
                ::opentelemetry::nostd::shared_ptr<::opentelemetry::trace::Span> span{
                    _tracer->StartSpan("message", ::opentelemetry::trace::StartSpanOptions
                    {
                        {}, {},
                        operation->GetContext()
                    })
                };
                ::boost::asio::post(_h_context, [this,
                    username = ::std::string{ arg1 },
                    channel = ::std::string{ arg2 },
                    content = ::std::string{ arg3 },
                    span]()->void {
                ::opentelemetry::trace::Scope scope{
                    _tracer->StartSpan("on_message", ::opentelemetry::trace::StartSpanOptions
                    {
                        {}, {},
                        span->GetContext()
                    })
                };
                _handler.on_message(message{
                    username,
                    content,
                    channel
                });

                }); // post
            } else if (::RE2::FullMatch(line, _re_ping,
                    &arg1)) {
                pong = true;
            } else if (::RE2::FullMatch(line, _re_reconnect,
                    &arg1)) {
                disconnect = true;
            }
        }
    }
    _buffer.consume(bytes_transferred);
    if (pong) {
        ::opentelemetry::nostd::shared_ptr<::opentelemetry::trace::Span> span{
            _tracer->StartSpan("on_pong", ::opentelemetry::trace::StartSpanOptions
            {
                {}, {},
                operation->GetContext()
            })
        };
        on_push("PONG :tmi.twitch.tv", span);
    }
    if (disconnect) {
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
template<typename Handler>
void connection<Handler>::on_push(::std::string&& request,
    ::opentelemetry::nostd::shared_ptr<::opentelemetry::trace::Span> root) {
    _q_write.emplace(::std::move(request));
    if (1 < _q_write.size()) {
        return;
    }
    on_write(root);
};
template<typename Handler>
void connection<Handler>::on_write(
    ::opentelemetry::nostd::shared_ptr<::opentelemetry::trace::Span> root) {
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
template<typename Handler>
void connection<Handler>::on_disconnect(
    ::opentelemetry::nostd::shared_ptr<::opentelemetry::trace::Span> root) {
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
template<typename Handler>
void connection<Handler>::on_send(
    ::opentelemetry::nostd::shared_ptr<::opentelemetry::trace::Span> root) {
    ::opentelemetry::nostd::shared_ptr<::opentelemetry::trace::Span> span{
        _tracer->StartSpan("on_send", ::opentelemetry::trace::StartSpanOptions
        {
            {}, {},
            root->GetContext()
        })
    };
    on_push("PRIVMSG #" + _message_channel + " :" + _message_content,
        span);
};
template<typename Handler>
void connection<Handler>::on_join(
    ::opentelemetry::nostd::shared_ptr<::opentelemetry::trace::Span> root) {
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
template<typename Handler>
void connection<Handler>::on_leave(
    ::opentelemetry::nostd::shared_ptr<::opentelemetry::trace::Span> root) {
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

} // detail
} // clients
} // chat
} // ai

#endif
