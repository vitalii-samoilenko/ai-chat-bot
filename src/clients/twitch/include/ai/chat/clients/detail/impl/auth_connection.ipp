#ifndef AI_CHAT_CLIENTS_DETAIL_AUTH_CONNECTION_IPP
#define AI_CHAT_CLIENTS_DETAIL_AUTH_CONNECTION_IPP

namespace ai {
namespace chat {
namespace clients {
namespace detail {

auth_connection::auth_connection()
    : _io_context{}
    , _resolver{ _io_context }
    , _ssl_context{ ::boost::asio::ssl::context::tlsv12_client }
    , _stream{ ::eboost::beast::metered_tcp_stream<auth_connection>{ ::eboost::beast::metered_rate_policy<auth_connection>{ *this }, _io_context }, _ssl_context }
    , _buffer{}
    , _context{ nullptr }
    , _tracer{
        ::opentelemetry::trace::Provider::GetTracerProvider()
            ->GetTracer("ai_chat_clients_auth")
    }
    , _meter{
        ::opentelemetry::metrics::Provider::GetMeterProvider()
            ->GetMeter("ai_chat_clients_auth")
    }
    , _m_network{ _meter->CreateUInt64Counter("ai_chat_clients_auth_network") }
    , _host{}
    , _port{}
    , _path{}
    , _t_validate{}
    , _t_token{}
    , _t_device{}
    , _h_oauth{ "OAuth " }
    , _timeout{} {

};

void auth_connection::bytes_rx(size_t n) {
    _m_network->Add(n, {
        {"type", "rx"}
    });
};
void auth_connection::bytes_tx(size_t n) {
    _m_network->Add(n, {
        {"type", "tx"}
    });
};

void auth_connection::on_init() {
    _t_validate = _path + "validate";
    _t_token = _path + "token";
    _t_device = _path + "device";
    _ssl_context.set_verify_mode(::boost::asio::ssl::verify_none);
};
template<typename Request, typename Response>
void auth_connection::on_send(Request &request, Response &response,
    ::opentelemetry::nostd::shared_ptr<::opentelemetry::trace::Span> root) {
    ::opentelemetry::nostd::shared_ptr<::opentelemetry::trace::Span> span{
        _tracer->StartSpan("on_send", ::opentelemetry::trace::StartSpanOptions{
            {}, {},
            root->GetContext()
        })
    };
    request.set(::boost::beast::http::field::host, _host);
    request.set(::boost::beast::http::field::user_agent, BOOST_BEAST_VERSION_STRING);
    if (!::SSL_set_tlsext_host_name(_stream.native_handle(), _host.c_str())) {
        throw ::boost::beast::system_error{
            static_cast<int>(::ERR_get_error()),
            ::boost::asio::error::get_ssl_category()
        };
    }
    _stream.set_verify_callback(::boost::asio::ssl::host_name_verification{ _host });
    ::boost::beast::get_lowest_layer(_stream).expires_after(_timeout);
    ::opentelemetry::nostd::shared_ptr<::opentelemetry::trace::Span> operation{
        _tracer->StartSpan("on_dns_resolve", ::opentelemetry::trace::StartSpanOptions{
            {}, {},
            span->GetContext()
        })
    };
    _resolver.async_resolve(_host, _port, [this, &request, &response, span, operation](::boost::beast::error_code error_code, ::boost::asio::ip::tcp::resolver::results_type results) mutable ->void {
    ::eboost::beast::ensure_success(error_code);
    operation = _tracer->StartSpan("on_tcp_connect", ::opentelemetry::trace::StartSpanOptions{
        {}, {},
        span->GetContext()
    });
    ::boost::beast::get_lowest_layer(_stream).async_connect(results, [this, &request, &response, span, operation](::boost::beast::error_code error_code, ::boost::asio::ip::tcp::resolver::results_type::endpoint_type) mutable ->void {
    ::eboost::beast::ensure_success(error_code);
    operation = _tracer->StartSpan("on_ssl_handshake", ::opentelemetry::trace::StartSpanOptions{
        {}, {},
        span->GetContext()
    });
    _stream.async_handshake(::boost::asio::ssl::stream_base::client, [this, &request, &response, span, operation](::boost::beast::error_code error_code) mutable ->void {
    ::eboost::beast::ensure_success(error_code);
    operation = _tracer->StartSpan("on_write", ::opentelemetry::trace::StartSpanOptions{
        {}, {},
        span->GetContext()
    });
    ::boost::beast::http::async_write(_stream, request, [this, &response, span, operation](::boost::beast::error_code error_code, size_t bytes_transferred) mutable ->void {
    ::eboost::beast::ensure_success(error_code);
    ::boost::ignore_unused(bytes_transferred);
    operation = _tracer->StartSpan("on_read", ::opentelemetry::trace::StartSpanOptions{
        {}, {},
        span->GetContext()
    });
    ::boost::beast::http::async_read(_stream, _buffer, response, [this, span, operation](::boost::beast::error_code error_code, size_t bytes_transferred) mutable ->void {
    ::eboost::beast::ensure_success(error_code);
    ::boost::ignore_unused(bytes_transferred);
    operation = nullptr;
    auto now = ::std::chrono::steady_clock::now();
    _next = now.time_since_epoch() + _delay;
    operation = _tracer->StartSpan("on_shutdown", ::opentelemetry::trace::StartSpanOptions{
        {}, {},
        span->GetContext()
    });
    _stream.async_shutdown([this, span, operation](::boost::beast::error_code error_code) mutable ->void {
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
void auth_connection::_stream_reset() {
    _stream.~stream();
    new(&_stream) ::boost::asio::ssl::stream<::eboost::beast::metered_tcp_stream<auth_connection>>{
        ::eboost::beast::metered_tcp_stream<auth_connection>{ ::eboost::beast::metered_rate_policy<auth_connection>{ *this }, _io_context },
        _ssl_context
    };
};

} // detail
} // clients
} // chat
} // ai

#endif
