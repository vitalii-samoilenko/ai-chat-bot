#ifndef AI_CHAT_ADAPTERS_DETAIL_CONNECTION_IPP
#define AI_CHAT_ADAPTERS_DETAIL_CONNECTION_IPP

namespace ai {
namespace chat {
namespace adapters {
namespace detail {

connection::connection()
    : _io_context{}
    , _resolver{ _io_context }
    , _ssl_context{ ::boost::asio::ssl::context::tlsv12_client }
    , _stream{ ::eboost::beast::metered_tcp_stream<connection>{ ::eboost::beast::metered_rate_policy<connection>{ *this }, _io_context }, _ssl_context }
    , _buffer{}
    , _completion{ ::boost::json::object_kind }
    , _host{}
    , _port{}
    , _t_completions{}
    , _h_bearer{ "Bearer " }
    , _timeout{}
    , _delay{}
    , _limit{}
    , _next{}
    INIT_LOGGER("ai_chat_adapters_openai")
    INIT_TRACER("ai_chat_adapters_openai")
    INIT_METER("ai_chat_adapters_openai")
    INIT_GAUGE(_m_context, "ai_chat_adapters_openai_context_total")
    INIT_COUNTER(_m_network, "ai_chat_adapters_openai_network") {

};

void connection::bytes_rx(size_t n) {
    ADD_COUNTER(_m_network, n, {
        TAG("type", "rx")
    })
};
void connection::bytes_tx(size_t n) {
    ADD_COUNTER(_m_network, n, {
        TAG("type", "tx")
    })
};

void connection::on_init() {
    _t_completions += "chat/completions";
    _ssl_context.set_verify_mode(::boost::asio::ssl::verify_none);
    ::boost::json::object &object{ _completion.as_object() };
    object.emplace("model", ::boost::json::string_kind);
    object.emplace("messages", ::boost::json::array_kind);
};
template<typename Request, typename Response>
void connection::on_send(Request &request, Response &response
    DECLARE_SPAN(root)) {
    START_SUBSPAN(span, "on_send", root, (*this))
    request.set(::boost::beast::http::field::host, _host);
    request.set(::boost::beast::http::field::user_agent, BOOST_BEAST_VERSION_STRING);
    if (!::SSL_set_tlsext_host_name(_stream.native_handle(), _host.c_str())) {
        throw ::boost::beast::system_error{
            static_cast<int>(::ERR_get_error()),
            ::boost::asio::error::get_ssl_category()
        };
    }
    _stream.set_verify_callback(::boost::asio::ssl::host_name_verification{ _host });
    auto now = ::std::chrono::steady_clock::now();
    if (now.time_since_epoch() < _next) {
        ::std::this_thread::sleep_for(_next - now.time_since_epoch());
    }
    ::boost::beast::get_lowest_layer(_stream).expires_after(_timeout);
    START_SUBSPAN(operation, "on_dns_resolve", span, (*this))
    _resolver.async_resolve(_host, _port, [this, &request, &response PROPAGATE_SPAN(span) PROPAGATE_SPAN(operation)](::boost::beast::error_code error_code, ::boost::asio::ip::tcp::resolver::results_type results) mutable ->void {
    ::eboost::beast::ensure_success(error_code);
    RESTART_SUBSPAN(operation, "on_tcp_connect", span, (*this))
    ::boost::beast::get_lowest_layer(_stream).async_connect(results, [this, &request, &response PROPAGATE_SPAN(span) PROPAGATE_SPAN(operation)](::boost::beast::error_code error_code, ::boost::asio::ip::tcp::resolver::results_type::endpoint_type) mutable ->void {
    ::eboost::beast::ensure_success(error_code);
    RESTART_SUBSPAN(operation, "on_ssl_handshake", span, (*this))
    _stream.async_handshake(::boost::asio::ssl::stream_base::client, [this, &request, &response PROPAGATE_SPAN(span) PROPAGATE_SPAN(operation)](::boost::beast::error_code error_code) mutable ->void {
    ::eboost::beast::ensure_success(error_code);
    RESTART_SUBSPAN(operation, "on_write", span, (*this))
    ::boost::beast::http::async_write(_stream, request, [this, &response PROPAGATE_SPAN(span) PROPAGATE_SPAN(operation)](::boost::beast::error_code error_code, size_t bytes_transferred) mutable ->void {
    ::eboost::beast::ensure_success(error_code);
    ::boost::ignore_unused(bytes_transferred);
    RESTART_SUBSPAN(operation, "on_read", span, (*this))
    ::boost::beast::http::async_read(_stream, _buffer, response, [this PROPAGATE_SPAN(span) PROPAGATE_SPAN(operation)](::boost::beast::error_code error_code, size_t bytes_transferred) mutable ->void {
    ::eboost::beast::ensure_success(error_code);
    ::boost::ignore_unused(bytes_transferred);
    STOP_SPAN(operation)
    auto now = ::std::chrono::steady_clock::now();
    _next = now.time_since_epoch() + _delay;
    RESTART_SUBSPAN(operation, "on_shutdown", span, (*this))
    _stream.async_shutdown([this PROPAGATE_SPAN(span) PROPAGATE_SPAN(operation)](::boost::beast::error_code error_code) mutable ->void {
    if (error_code == ::boost::asio::ssl::error::stream_truncated) {
        return;
    }
    ::eboost::beast::ensure_success(error_code);

    }); // shutdown
    }); // read
    }); // write
    }); // SSL handshake
    }); // TCP connect
    }); // resolve
};
void connection::_stream_reset() {
    _stream.~stream();
    new(&_stream) ::boost::asio::ssl::stream<::eboost::beast::metered_tcp_stream<connection>>{
        ::eboost::beast::metered_tcp_stream<connection>{ ::eboost::beast::metered_rate_policy<connection>{ *this }, _io_context },
        _ssl_context
    };
};

} // detail
} // adapters
} // chat
} // ai

#endif
