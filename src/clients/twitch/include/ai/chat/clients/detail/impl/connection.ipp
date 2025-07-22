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
    , _re_command_wargs{ R"(:(?<username>[a-z0-9_]+)![a-z0-9_]+@[a-z0-9_]+\.tmi\.twitch\.tv PRIVMSG #(?<channel>[a-z0-9_]+) :!(?<command>[a-z]+) (?<args>.+))" }
    , _re_command{ R"(:(?<username>[a-z0-9_]+)![a-z0-9_]+@[a-z0-9_]+\.tmi\.twitch\.tv PRIVMSG #(?<channel>[a-z0-9_]+) :!(?<command>[a-z]+))" }
    , _re_message{ R"(:(?<username>[a-z0-9_]+)![a-z0-9_]+@[a-z0-9_]+\.tmi\.twitch\.tv PRIVMSG #(?<channel>[a-z0-9_]+) :(?<message>.+))" }
    , _re_ping{ R"(PING (?<ping>:tmi\.twitch\.tv))" }
    , _re_reconnect{ R"((?<reconnect>:tmi\.twitch\.tv) RECONNECT)" }
    INIT_LOGGER("ai_chat_clients_twitch")
    INIT_TRACER("ai_chat_clients_twitch")
    INIT_METER("ai_chat_clients_twitch")
    INIT_COUNTER(_m_network, "ai_chat_clients_twitch_network") {

};

template<typename Handler>
void connection<Handler>::bytes_rx(size_t n) {
    ADD_COUNTER(_m_network, n, {
        TAG("type", "rx")
    })
};
template<typename Handler>
void connection<Handler>::bytes_tx(size_t n) {
    ADD_COUNTER(_m_network, n, {
        TAG("type", "tx")
    })
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
    START_SPAN(span, "on_signal", (*this))
    on_disconnect(
        PROPAGATE_ONLY_SPAN(span));

    }); // SIG
    ::boost::asio::post(_io_run_context, [this]()->void {
    _io_context.run();

    }); // post
};
template<typename Handler>
void connection<Handler>::on_connect(
    DECLARE_ONLY_SPAN(root)) {
    START_SUBSPAN(span, "on_connect", root, (*this))
    if (!::SSL_set_tlsext_host_name(_stream.next_layer().native_handle(), _host.c_str())) {
        throw ::boost::beast::system_error{
            static_cast<int>(::ERR_get_error()),
            ::boost::asio::error::get_ssl_category()
        };
    }
    _stream.next_layer().set_verify_callback(::boost::asio::ssl::host_name_verification{ _host });
    ::boost::beast::get_lowest_layer(_stream).expires_after(_timeout);
    START_SUBSPAN(operation, "on_dns_resolve", span, (*this))
    _resolver.async_resolve(_host, _port, [this PROPAGATE_SPAN(span) PROPAGATE_SPAN(operation)](::boost::beast::error_code error_code, ::boost::asio::ip::tcp::resolver::results_type results) mutable ->void {
    if (error_code == ::boost::beast::errc::operation_canceled) {
        return;
    }
    ::eboost::beast::ensure_success(error_code);
    RESTART_SUBSPAN(operation, "on_tcp_connect", span, (*this))
    ::boost::beast::get_lowest_layer(_stream).async_connect(results, [this PROPAGATE_SPAN(span) PROPAGATE_SPAN(operation)](::boost::beast::error_code error_code, ::boost::asio::ip::tcp::resolver::results_type::endpoint_type endpoint_type) mutable ->void {
    if (error_code == ::boost::beast::errc::operation_canceled) {
        return;
    }
    ::eboost::beast::ensure_success(error_code);
    RESTART_SUBSPAN(operation, "on_ssl_handshake", span, (*this))
    _stream.next_layer().async_handshake(::boost::asio::ssl::stream_base::client, [this PROPAGATE_SPAN(span) PROPAGATE_SPAN(operation)](::boost::beast::error_code error_code) mutable ->void {
    if (error_code == ::boost::beast::errc::operation_canceled) {
        return;
    }
    ::eboost::beast::ensure_success(error_code);
    STOP_SPAN(operation)
    ::boost::beast::get_lowest_layer(_stream).expires_never();
    _stream.set_option(::boost::beast::websocket::stream_base::timeout::suggested(::boost::beast::role_type::client));
    RESTART_SUBSPAN(operation, "on_ws_handshake", span, (*this))
    _stream.async_handshake(_authority, _path, [this PROPAGATE_SPAN(span) PROPAGATE_SPAN(operation)](::boost::beast::error_code error_code) mutable ->void {
    if (error_code == ::boost::beast::errc::operation_canceled) {
        return;
    }
    ::eboost::beast::ensure_success(error_code);
    STOP_SPAN(operation)
    char const PASS[]{ "PASS oauth:" };
    ::boost::asio::mutable_buffer request{ _buffer.prepare(::std::size(PASS) - 1 + _access_token.size()) };
    ::std::memcpy(reinterpret_cast<char *>(request.data()), PASS, ::std::size(PASS) - 1);
    ::std::memcpy(reinterpret_cast<char *>(request.data()) + ::std::size(PASS) - 1, _access_token.data(), _access_token.size());
    RESTART_SUBSPAN(operation, "on_pass", span, (*this))
    _stream.async_write(request, [this PROPAGATE_SPAN(span) PROPAGATE_SPAN(operation)](::boost::beast::error_code error_code, size_t bytes_transferred) mutable ->void {
    if (error_code == ::boost::beast::errc::operation_canceled) {
        return;
    }
    ::eboost::beast::ensure_success(error_code);
    ::boost::ignore_unused(bytes_transferred);
    STOP_SPAN(operation)
    char const NICK[]{ "NICK " };
    ::boost::asio::mutable_buffer request{ _buffer.prepare(::std::size(NICK) - 1 + _username.size()) };
    ::std::memcpy(reinterpret_cast<char *>(request.data()), NICK, ::std::size(NICK) - 1);
    ::std::memcpy(reinterpret_cast<char *>(request.data()) + ::std::size(NICK) - 1, _username.data(), _username.size());
    RESTART_SUBSPAN(operation, "on_nick", span, (*this))
    _stream.async_write(request, [this PROPAGATE_SPAN(span) PROPAGATE_SPAN(operation)](::boost::beast::error_code error_code, size_t bytes_transferred) mutable ->void {
    if (error_code == ::boost::beast::errc::operation_canceled) {
        return;
    }
    ::eboost::beast::ensure_success(error_code);
    ::boost::ignore_unused(bytes_transferred);
    RESTART_SUBSPAN(operation, "on_notice", span, (*this))
    _stream.async_read(_buffer, [this PROPAGATE_SPAN(span) PROPAGATE_SPAN(operation)](::boost::beast::error_code error_code, size_t bytes_transferred) mutable ->void {
    if (error_code == ::boost::beast::errc::operation_canceled) {
        return;
    }
    ::eboost::beast::ensure_success(error_code);
    ::boost::asio::const_buffer response{ _buffer.cdata() };
    LOG_INFO(reinterpret_cast<char const *>(response.data()), bytes_transferred, operation, (*this))
    {
        ::std::string_view notice{};
        ::std::string_view cursor{ reinterpret_cast<char const *>(response.data()), bytes_transferred };
        if (::RE2::FullMatch(cursor, _re_notice,
                &notice)) {
            throw ::std::invalid_argument{ ::std::string{ notice } };
        }
    }
    _buffer.consume(bytes_transferred);
    STOP_SPAN(operation)
    char const JOIN[]{ "JOIN #" };
    char const AND[]{ ",#" };
    ::boost::asio::mutable_buffer request{ _buffer.prepare(::std::size(JOIN) - 1 + _username.size() + (!_channel.empty() ? ::std::size(AND) - 1 + _channel.size() : 0)) };
    ::std::memcpy(reinterpret_cast<char *>(request.data()), JOIN, ::std::size(JOIN) - 1);
    ::std::memcpy(reinterpret_cast<char *>(request.data()) + ::std::size(JOIN) - 1, _username.data(), _username.size());
    if (!_channel.empty()) {
        ::std::memcpy(reinterpret_cast<char *>(request.data()) + ::std::size(JOIN) - 1 + _username.size(), AND, ::std::size(AND) - 1);
        ::std::memcpy(reinterpret_cast<char *>(request.data()) + ::std::size(JOIN) - 1 + _username.size() + ::std::size(AND) - 1, _channel.data(), _channel.size());
    }
    RESTART_SUBSPAN(operation, "on_join", span, (*this))
    _stream.async_write(request, [this PROPAGATE_SPAN(span) PROPAGATE_SPAN(operation)](::boost::beast::error_code error_code, size_t bytes_transferred) mutable ->void {
    if (error_code == ::boost::beast::errc::operation_canceled) {
        return;
    }
    ::eboost::beast::ensure_success(error_code);
    ::boost::ignore_unused(bytes_transferred);
    STOP_SPAN(operation)
    STOP_SPAN(span)
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
    START_SPAN(operation, "on_read", (*this))
    _stream.async_read(_buffer, [this PROPAGATE_SPAN(operation)](::boost::beast::error_code error_code, size_t bytes_transferred) mutable ->void {
    if (error_code == ::boost::beast::errc::operation_canceled) {
        return;
    }
    ::eboost::beast::ensure_success(error_code);
    ::boost::asio::const_buffer response{ _buffer.cdata() };
    LOG_INFO(reinterpret_cast<char const *>(response.data()), bytes_transferred, operation, (*this));
    bool pong{ false };
    bool disconnect{ false };
    {
        ::std::string_view line{};
        ::std::string_view cursor{ reinterpret_cast<char const *>(response.data()), bytes_transferred };
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
                START_SUBSPAN(span, "command", operation, (*this))
                ::boost::asio::post(_h_context, [this,
                    username = ::std::string{ arg1 },
                    channel = ::std::string{ arg2 },
                    name = ::std::string{ arg3 },
                    args = ::std::string{ arg4 }
                    PROPAGATE_SPAN(span)]()->void {
                START_SUBSCOPE(scope, "on_command", span, (*this))
                _handler.on_command(command{
                    username,
                    name, args,
                    channel
                });

                }); // post
            } else if (::RE2::FullMatch(line, _re_message,
                    &arg1, &arg2, &arg3)) {
                START_SUBSPAN(span, "message", operation, (*this))
                ::boost::asio::post(_h_context, [this,
                    username = ::std::string{ arg1 },
                    channel = ::std::string{ arg2 },
                    content = ::std::string{ arg3 }
                    PROPAGATE_SPAN(span)]()->void {
                START_SUBSCOPE(scope, "on_message", span, (*this))
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
        START_SUBSPAN(span, "on_pong", operation, (*this))
        on_push("PONG :tmi.twitch.tv"
            PROPAGATE_SPAN(span));
    }
    if (disconnect) {
        START_SUBSPAN(span, "reconnect", operation, (*this))
        RESTART_SUBSPAN(operation, "on_close", span, (*this))
        _stream.async_close(::boost::beast::websocket::close_code::normal, [this PROPAGATE_SPAN(span) PROPAGATE_SPAN(operation)](::boost::beast::error_code error_code) mutable ->void {
        if (error_code == ::boost::beast::errc::operation_canceled) {
            return;
        }
        ::eboost::beast::ensure_success(error_code);
        STOP_SPAN(operation)
        _stream.~stream();
        new(&_stream) ::boost::beast::websocket::stream<::boost::asio::ssl::stream<::eboost::beast::metered_tcp_stream<connection>>>{
            ::eboost::beast::metered_tcp_stream<connection>{ ::eboost::beast::metered_rate_policy<connection>{ *this }, _io_context },
            _ssl_context
        };
        on_connect(
            PROPAGATE_ONLY_SPAN(span));

        }); // close
    } else {
        STOP_SPAN(operation)
        on_read();
    }

    }); // read
};
template<typename Handler>
void connection<Handler>::on_push(::std::string&& request
    DECLARE_SPAN(root)) {
    _q_write.emplace(::std::move(request));
    if (1 < _q_write.size()) {
        return;
    }
    on_write(
        PROPAGATE_ONLY_SPAN(root));
};
template<typename Handler>
void connection<Handler>::on_write(
    DECLARE_ONLY_SPAN(root)) {
    auto now = ::std::chrono::steady_clock::now();
    if (now.time_since_epoch() < _next) {
        ::std::this_thread::sleep_for(_next - now.time_since_epoch());
    }
    START_SUBSPAN(operation, "on_write", root, (*this))
    _stream.async_write(::boost::asio::buffer(_q_write.front()), [this PROPAGATE_SPAN(root) PROPAGATE_SPAN(operation)](::boost::beast::error_code error_code, size_t bytes_transferred) mutable ->void {
    if (error_code == ::boost::beast::errc::operation_canceled) {
        return;
    }
    ::eboost::beast::ensure_success(error_code);
    ::boost::ignore_unused(bytes_transferred);
    STOP_SPAN(operation)
    auto now = ::std::chrono::steady_clock::now();
    _next = now.time_since_epoch() + _delay;
    _q_write.pop();
    if (_q_write.empty()) {
        return;
    }
    on_write(
        PROPAGATE_ONLY_SPAN(root));

    }); // write
};
template<typename Handler>
void connection<Handler>::on_disconnect(
    DECLARE_ONLY_SPAN(root)) {
    START_SUBSPAN(span, "on_disconnect", root, (*this))
    START_SUBSPAN(operation, "on_close", span, (*this))
    _stream.async_close(::boost::beast::websocket::close_code::normal, [this PROPAGATE_SPAN(span) PROPAGATE_SPAN(operation)](::boost::beast::error_code error_code) mutable ->void {
    STOP_SPAN(operation)
    _io_context.stop();
    _io_run_context.stop();
    _h_context.stop();

    }); // close
};
template<typename Handler>
void connection<Handler>::on_send(
    DECLARE_ONLY_SPAN(root)) {
    START_SUBSPAN(span, "on_send", root, (*this))
    on_push("PRIVMSG #" + _message_channel + " :" + _message_content
        PROPAGATE_SPAN(span));
};
template<typename Handler>
void connection<Handler>::on_join(
    DECLARE_ONLY_SPAN(root)) {
    START_SUBSPAN(span, "on_join", root, (*this))
    on_push("JOIN #" + _channel
        PROPAGATE_SPAN(span));
};
template<typename Handler>
void connection<Handler>::on_leave(
    DECLARE_ONLY_SPAN(root)) {
    START_SUBSPAN(span, "on_leave", root, (*this))
    on_push("PART #" + _channel
        PROPAGATE_SPAN(span));
};

} // detail
} // clients
} // chat
} // ai

#endif
