#ifndef AI_CHAT_ADAPTERS_DETAIL_CONNECTION_IPP
#define AI_CHAT_ADAPTERS_DETAIL_CONNECTION_IPP

#include <thread>

#include "ai/chat/adapters/detail/connection.hpp"

namespace ai {
namespace chat {
namespace adapters {
namespace detail {

connection::connection()
    : // _buffer{},
      _io_context{}
    , _dns_resolver{ _io_context }
    , _ssl_context{ ::boost::asio::ssl::context::tlsv12_client }
    , _ssl_stream{
        ::eboost::beast::metered_tcp_stream<connection>{ ::eboost::beast::metered_rate_policy<connection>{ *this }, _io_context },
        _ssl_context
    }
    , _request{}
    , _response{}
    , _request_serializer{ _request }
    , _response_parser{ &_buffer[0], RESPONSE_SIZE, true }
    , _read_buffer{ &_buffer[RESPONSE_SIZE], READ_SIZE }
    , _request_body{ nullptr }
    , _response_body{ nullptr }
    , _json_serializer{}
    , _json_parser{
        ::boost::json::get_null_resource(),
        ::boost::json::parse_options{},
        reinterpret_cast<unsigned char *>(&_buffer[RESPONSE_SIZE + READ_SIZE]), JSON_SIZE
    }
    , _host{}
    , _port{}
    , _t_completions{}
    , _h_bearer{ "Bearer " }
    , _timeout{}
    , _delay{}
    , _next{}
    , _total_limit{}
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
    _ssl_context.set_verify_mode(::boost::asio::ssl::verify_none);
    _t_completions += "chat/completions";
};
void connection::on_send(
    DECLARE_ONLY_SPAN(root)) {
    START_SUBSPAN(span, "on_send", root, (*this))
    if (!::SSL_set_tlsext_host_name(_ssl_stream.native_handle(), _host.c_str())) {
        throw ::boost::beast::system_error{
            static_cast<int>(::ERR_get_error()),
            ::boost::asio::error::get_ssl_category()
        };
    }
    _ssl_stream.set_verify_callback(::boost::asio::ssl::host_name_verification{ _host });
    auto now = ::std::chrono::steady_clock::now();
    if (now.time_since_epoch() < _next) {
        ::std::this_thread::sleep_for(_next - now.time_since_epoch());
    }
    ::boost::beast::get_lowest_layer(_ssl_stream).expires_after(_timeout);
    START_SUBSPAN(operation, "on_dns_resolve", span, (*this))
    _dns_resolver.async_resolve(_host, _port, [this PROPAGATE_SPAN(span) PROPAGATE_SPAN(operation)](::boost::beast::error_code error_code, ::boost::asio::ip::tcp::resolver::results_type results) mutable ->void {
    ::eboost::beast::ensure_success(error_code);
    RESTART_SUBSPAN(operation, "on_tcp_connect", span, (*this))
    ::boost::beast::get_lowest_layer(_ssl_stream).async_connect(results, [this PROPAGATE_SPAN(span) PROPAGATE_SPAN(operation)](::boost::beast::error_code error_code, ::boost::asio::ip::tcp::resolver::results_type::endpoint_type) mutable ->void {
    ::eboost::beast::ensure_success(error_code);
    RESTART_SUBSPAN(operation, "on_ssl_handshake", span, (*this))
    _ssl_stream.async_handshake(::boost::asio::ssl::stream_base::client, [this PROPAGATE_SPAN(span) PROPAGATE_SPAN(operation)](::boost::beast::error_code error_code) mutable ->void {
    ::eboost::beast::ensure_success(error_code);
    RESTART_SUBSPAN(operation, "on_write_headers", span, (*this))
    ::boost::beast::http::async_write_header(_ssl_stream, _request_serializer, [this PROPAGATE_SPAN(span) PROPAGATE_SPAN(operation)](::boost::beast::error_code error_code, size_t bytes_transferred) mutable ->void {
    ::eboost::beast::ensure_success(error_code);
    ::boost::ignore_unused(bytes_transferred);
    STOP_SPAN(operation)
    on_write_chunk(
        PROPAGATE_ONLY_SPAN(span));

    }); // write headers
    }); // SSL handshake
    }); // TCP connect
    }); // DNS resolve
};
void connection::on_write_chunk(
    DECLARE_ONLY_SPAN(span)) {
    START_SUBSPAN(operation, "on_write_chunk", span, (*this))
    ::std::string_view chunk{ _json_serializer.read(&_buffer[0], BUFFER_SIZE) };
    _request.body().data = const_cast<char *>(chunk.data());
    _request.body().size = chunk.size();
    _request.body().more = !_json_serializer.done();
    LOG_INFO(chunk.data(), chunk.size(), operation, (*this));
    ::boost::beast::http::async_write_some(_ssl_stream, _request_serializer, [this PROPAGATE_SPAN(span) PROPAGATE_SPAN(operation)](::boost::beast::error_code error_code, size_t bytes_transferred) mutable ->void {
    if (error_code == ::boost::beast::http::error::need_buffer) {
        STOP_SPAN(operation)
        on_write_chunk(
            PROPAGATE_ONLY_SPAN(span));
        return;
    }
    ::eboost::beast::ensure_success(error_code);
    ::boost::ignore_unused(bytes_transferred);
    RESTART_SUBSPAN(operation, "on_read_headers", span, (*this))
    ::boost::beast::http::async_read_header(_ssl_stream, _read_buffer, _response_parser, [this PROPAGATE_SPAN(span) PROPAGATE_SPAN(operation)](::boost::beast::error_code error_code, size_t bytes_transferred) mutable ->void {
    ::eboost::beast::ensure_success(error_code);
    ::boost::ignore_unused(bytes_transferred);
    STOP_SPAN(operation)
    on_read_chunk(
        PROPAGATE_ONLY_SPAN(span));

    }); // read headers    
    }); // write chunk
};
void connection::on_read_chunk(
    DECLARE_ONLY_SPAN(span)) {
    START_SUBSPAN(operation, "on_read_chunk", span, (*this))
    ::boost::beast::http::async_read_some(_ssl_stream, _read_buffer, _response_parser, [this PROPAGATE_SPAN(span) PROPAGATE_SPAN(operation)](::boost::beast::error_code error_code, size_t bytes_transferred) mutable ->void {
    if (!(error_code == ::boost::beast::http::error::need_buffer)) {
        ::eboost::beast::ensure_success(error_code);
    }
    LOG_INFO(&_buffer[0], bytes_transferred, operation, (*this));
    _json_parser.write_some(&_buffer[0], bytes_transferred);
    ::boost::beast::http::response<::boost::beast::http::buffer_body> &response{ _response_parser.get() };
    response.body().data = &_buffer[0];
    response.body().size = RESPONSE_SIZE;
    STOP_SPAN(operation)
    if (error_code == ::boost::beast::http::error::need_buffer) {
        on_read_chunk(
            PROPAGATE_ONLY_SPAN(span));
        return;
    }
    _response = _response_parser.release();
    _response_body = _json_parser.release();
    auto now = ::std::chrono::steady_clock::now();
    _next = now.time_since_epoch() + _delay;
    RESTART_SUBSPAN(operation, "on_shutdown", span, (*this))
    _ssl_stream.async_shutdown([this PROPAGATE_SPAN(span) PROPAGATE_SPAN(operation)](::boost::beast::error_code error_code) mutable ->void {
    if (error_code == ::boost::asio::ssl::error::stream_truncated) {
        return;
    }
    ::eboost::beast::ensure_success(error_code);

    }); // shutdown
    }); // read chunk
};

void connection::on_reset() {
    _io_context.restart();
    _ssl_stream.~stream();
    new(&_ssl_stream) ::boost::asio::ssl::stream<::eboost::beast::metered_tcp_stream<connection>>{
        ::eboost::beast::metered_tcp_stream<connection>{ ::eboost::beast::metered_rate_policy<connection>{ *this }, _io_context },
        _ssl_context
    };
    _request = ::boost::beast::http::request<::boost::beast::http::buffer_body>{};
    _response = ::boost::beast::http::response<::boost::beast::http::buffer_body>{};
    _request_serializer.~serializer();
    new(&_request_serializer) ::boost::beast::http::request_serializer<::boost::beast::http::buffer_body>{ _request };
    _response_parser.~parser();
    new(&_response_parser) ::boost::beast::http::response_parser<::boost::beast::http::buffer_body>{ &_buffer[0], RESPONSE_SIZE, true };
    _json_serializer.reset(&_request_body);
    _json_parser.reset();
};

} // detail
} // adapters
} // chat
} // ai

#endif
