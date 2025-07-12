#ifndef AI_CHAT_CLIENTS_AUTH_IPP
#define AI_CHAT_CLIENTS_AUTH_IPP

#include <array>
#include <memory>
#include <stdexcept>

#include "boost/asio/buffer.hpp"
#include "boost/asio/ssl.hpp"
#include "boost/beast.hpp"
#include "eboost/beast/ensure_success.hpp"
#include "eboost/beast/http/form_body.hpp"
#include "eboost/beast/http/json_body.hpp"
#include "eboost/beast/metered_rate_policy.hpp"
#include "opentelemetry/metrics/provider.h"
#include "opentelemetry/trace/provider.h"
#include "re2/re2.h"

#include "ai/chat/clients/auth.hpp"

namespace ai {
namespace chat {
namespace clients {

class auth::connection {
public:
    // connection() = delete;
    connection(const connection&) = delete;
    connection(connection&&) = delete;

    ~connection() = default;

    connection& operator=(const connection&) = delete;
    connection& operator=(connection&&) = delete;

private:
    friend ::eboost::beast::metered_rate_policy<connection>;
    friend auth;

    connection()
        : _io_context{}
        , _resolver{ _io_context }
        , _ssl_context{ ::boost::asio::ssl::context::tlsv12_client }
        , _stream{ ::eboost::beast::metered_tcp_stream<connection>{ ::eboost::beast::metered_rate_policy<connection>{ *this }, _io_context }, _ssl_context }
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
        , _timeout{}
        , _buffer{} {

    };

    ::boost::asio::io_context _io_context;
    ::boost::asio::ip::tcp::resolver _resolver;
    ::boost::asio::ssl::context _ssl_context;
    ::boost::asio::ssl::stream<::eboost::beast::metered_tcp_stream<connection>> _stream;
    ::opentelemetry::nostd::shared_ptr<::opentelemetry::trace::Tracer> _tracer;
    ::opentelemetry::nostd::shared_ptr<::opentelemetry::metrics::Meter> _meter;
    ::opentelemetry::nostd::unique_ptr<::opentelemetry::metrics::Counter<uint64_t>> _m_network;
    ::std::string _host;
    ::std::string _port;
    ::std::string _path;
    ::std::chrono::milliseconds _timeout;
    ::boost::beast::flat_buffer _buffer;

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
        _ssl_context.set_verify_mode(::boost::asio::ssl::verify_none);
    };
    template<typename Request, typename Response>
    void on_send(Request& request, Response& response,
        ::opentelemetry::nostd::shared_ptr<::opentelemetry::trace::Span> root) {
        ::opentelemetry::nostd::shared_ptr<::opentelemetry::trace::Span> span{
            _tracer->StartSpan("on_send", ::opentelemetry::trace::StartSpanOptions
            {
                {}, {},
                root->GetContext()
            })
        };
        ::std::string target{ _path };
        request.target(target.append(request.target()));
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
            _tracer->StartSpan("on_dns_resolve", ::opentelemetry::trace::StartSpanOptions
            {
                {}, {},
                span->GetContext()
            })
        };
        _resolver.async_resolve(_host, _port, [this, &request, &response, span, operation](::boost::beast::error_code error_code, ::boost::asio::ip::tcp::resolver::results_type results) mutable ->void {
        ::eboost::beast::ensure_success(error_code);
        operation = _tracer->StartSpan("on_tcp_connect", ::opentelemetry::trace::StartSpanOptions
        {
            {}, {},
            span->GetContext()
        });
        ::boost::beast::get_lowest_layer(_stream).async_connect(results, [this, &request, &response, span, operation](::boost::beast::error_code error_code, ::boost::asio::ip::tcp::resolver::results_type::endpoint_type) mutable ->void {
        ::eboost::beast::ensure_success(error_code);
        operation = _tracer->StartSpan("on_ssl_handshake", ::opentelemetry::trace::StartSpanOptions
        {
            {}, {},
            span->GetContext()
        });
        _stream.async_handshake(::boost::asio::ssl::stream_base::client, [this, &request, &response, span, operation](::boost::beast::error_code error_code) mutable ->void {
        ::eboost::beast::ensure_success(error_code);
        operation = _tracer->StartSpan("on_write", ::opentelemetry::trace::StartSpanOptions
        {
            {}, {},
            span->GetContext()
        });
        ::boost::beast::http::async_write(_stream, request, [this, &response, span, operation](::boost::beast::error_code error_code, size_t bytes_transferred) mutable ->void {
        ::eboost::beast::ensure_success(error_code);
        ::boost::ignore_unused(bytes_transferred);
        operation = _tracer->StartSpan("on_read", ::opentelemetry::trace::StartSpanOptions
        {
            {}, {},
            span->GetContext()
        });
        ::boost::beast::http::async_read(_stream, _buffer, response, [this, span, operation](::boost::beast::error_code error_code, size_t bytes_transferred) mutable ->void {
        ::eboost::beast::ensure_success(error_code);
        ::boost::ignore_unused(bytes_transferred);
        operation = _tracer->StartSpan("on_shutdown", ::opentelemetry::trace::StartSpanOptions
        {
            {}, {},
            span->GetContext()
        });
        _stream.async_shutdown([this, span, operation](::boost::beast::error_code error_code) mutable ->void {
        operation = nullptr;
        _stream.~stream();
        new(&_stream) ::boost::asio::ssl::stream<::eboost::beast::metered_tcp_stream<connection>>{
            ::eboost::beast::metered_tcp_stream<connection>{ ::eboost::beast::metered_rate_policy<connection>{ *this }, _io_context },
            _ssl_context
        };
        if (error_code == ::boost::asio::ssl::error::stream_truncated) {
            return;
        }
        ::eboost::beast::ensure_success(error_code);

        }); // shutdown
        }); // read
        }); // write
        }); // handshake
        }); // connect
        }); // resolve
    };
};

auth::auth(const ::std::string& address, ::std::chrono::milliseconds timeout)
    : _service{ new connection{} } {
    ::RE2 uri{ R"(https:\/\/(?<host>[^:\/]+)(?::(?<port>\d+))?(?<path>\/.*))" };
    if (!::RE2::FullMatch(address, uri,
            &_service->_host, &_service->_port, &_service->_path)) {
        throw ::std::invalid_argument{ "invalid uri" };
    }
    if (_service->_port.empty()) {
        _service->_port = "443";
    }
    _service->_timeout = timeout;
    _service->on_init();
};

token_context tag_invoke(::boost::json::value_to_tag<token_context>, const ::boost::json::value& value) {
    return {
        ::boost::json::value_to<::std::string>(value.at("access_token")),
        ::boost::json::value_to<::std::string>(value.at("refresh_token")),
    };
};
access_context tag_invoke(::boost::json::value_to_tag<access_context>, const ::boost::json::value& value) {
    return {
        ::boost::json::value_to<::std::string>(value.at("device_code")),
        ::boost::json::value_to<::std::string>(value.at("verification_uri")),
    };
};

bool auth::validate_token(const ::std::string& token) {
    ::opentelemetry::nostd::shared_ptr<::opentelemetry::trace::Span> span{
        _service->_tracer->StartSpan("validate_token")
    };
    ::boost::beast::http::request<::boost::beast::http::empty_body> request{
        ::boost::beast::http::verb::get, "validate", 11,
    };
    request.set(::boost::beast::http::field::authorization, "OAuth " + token);
    ::boost::beast::http::response<::eboost::beast::http::json_body> response{};
    _service->on_send(request, response,
        span);
    _service->_io_context.run();
    _service->_io_context.restart();
    return response.result() == ::boost::beast::http::status::ok;
};

token_context auth::refresh_token(const ::std::string& client_id, const ::std::string& client_secret, const ::std::string& refresh_token) {
    ::opentelemetry::nostd::shared_ptr<::opentelemetry::trace::Span> span{
        _service->_tracer->StartSpan("refresh_token")
    };
    ::boost::beast::http::request<::eboost::beast::http::form_body<::std::array<::std::pair<::std::string, ::std::string>, 4>>> request{
        ::boost::beast::http::verb::post, "token", 11,
        ::std::array<::std::pair<::std::string, ::std::string>, 4>{
            ::std::make_pair("grant_type", "refresh_token"),
            ::std::make_pair("client_id", client_id),
            ::std::make_pair("client_secret", client_secret),
            ::std::make_pair("refresh_token", refresh_token),
        }
    };
    request.prepare_payload();
    request.set(::boost::beast::http::field::content_type, "application/x-www-form-urlencoded");
    ::boost::beast::http::response<::eboost::beast::http::json_body> response{};
    _service->on_send(request, response,
        span);
    _service->_io_context.run();
    _service->_io_context.restart();
    return ::boost::json::value_to<token_context>(response.body());
};

token_context auth::issue_token(const ::std::string& client_id, const ::std::string& device_code, const ::std::string& scopes) {
    ::opentelemetry::nostd::shared_ptr<::opentelemetry::trace::Span> span{
        _service->_tracer->StartSpan("issue_token")
    };
    ::boost::beast::http::request<::boost::beast::http::empty_body> request{
        ::boost::beast::http::verb::post, "token?grant_type=urn:ietf:params:oauth:grant-type:device_code&client_id=" + client_id + "&device_code=" + device_code + "&scopes=" + scopes, 11,
    };
    ::boost::beast::http::response<::eboost::beast::http::json_body> response{};
    _service->on_send(request, response,
        span);
    _service->_io_context.run();
    _service->_io_context.restart();
    return ::boost::json::value_to<token_context>(response.body());
};

access_context auth::request_access(const ::std::string& client_id, const ::std::string& scopes) {
    ::opentelemetry::nostd::shared_ptr<::opentelemetry::trace::Span> span{
        _service->_tracer->StartSpan("request_access")
    };
    ::boost::beast::http::request<::boost::beast::http::empty_body> request{
        ::boost::beast::http::verb::post, "device?client_id=" + client_id + "&scopes=" + scopes, 11,
    };
    ::boost::beast::http::response<::eboost::beast::http::json_body> response{};
    _service->on_send(request, response,
        span);
    _service->_io_context.run();
    _service->_io_context.restart();
    return ::boost::json::value_to<access_context>(response.body());
};

} // clients
} // chat
} // ai

#endif
