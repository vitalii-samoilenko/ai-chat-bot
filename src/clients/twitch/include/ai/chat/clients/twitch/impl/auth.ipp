#ifndef AI_CHAT_CLIENTS_TWITCH_AUTH_IPP
#define AI_CHAT_CLIENTS_TWITCH_AUTH_IPP

#include <array>
#include <stdexcept>

#include "boost/asio/buffer.hpp"
#include "boost/asio/ssl.hpp"
#include "boost/beast.hpp"
#include "boost/url.hpp"

#include "eboost/beast/http/form_body.hpp"
#include "eboost/beast/http/json_body.hpp"

#include "ai/chat/clients/twitch/auth.hpp"

namespace ai {
namespace chat {
namespace clients {
namespace twitch {

class auth::connection {
    friend auth;
    
public:
    // connection() = delete;
    connection(const connection&) = delete;
    connection(connection&&) = delete;

    ~connection() = default;

    connection& operator=(const connection&) = delete;
    connection& operator=(connection&&) = delete;

private:

    connection()
        : _context{}
        , _resolver{ _context }
        , _ssl_context{ ::boost::asio::ssl::context::tlsv12_client }
        , _stream{ _context, _ssl_context }
        , _host{}
        , _port{}
        , _path{}
        , _timeout{}
        , _buffer{} {

    };

    ::boost::asio::io_context _context;
    ::boost::asio::ip::tcp::resolver _resolver;
    ::boost::asio::ssl::context _ssl_context;
    ::boost::asio::ssl::stream<::boost::beast::tcp_stream> _stream;
    ::std::string _host;
    ::std::string _port;
    ::std::string _path;
    ::std::chrono::milliseconds _timeout;
    ::boost::beast::flat_buffer _buffer;

    void on_init() {
        _ssl_context.set_verify_mode(::boost::asio::ssl::verify_none);
        if (!::SSL_set_tlsext_host_name(_stream.native_handle(), _host.c_str())) {
            throw ::boost::beast::system_error{
                static_cast<int>(::ERR_get_error()),
                ::boost::asio::error::get_ssl_category()
            };
        }
        _stream.set_verify_callback(::boost::asio::ssl::host_name_verification{ _host });
    };
    template<typename Request, typename Response>
    void on_connect(Request& _request, Response& _response) {
        _request.target(_target + _request.target());
        _request.set(::boost::beast::http::field::host, _host);
        _request.set(::boost::beast::http::field::user_agent, BOOST_BEAST_VERSION_STRING);

        ::boost::beast::get_lowest_layer(_stream).expires_after(_timeout);
        _resolver.async_resolve(_host, _port,
            ::boost::beast::bind_front_handler(&on_resolve<Request, Response>, this, _request, _response));
    };
    template<typename Request, typename Response>
    void on_resolve(Request& _request, Response& _response, ::boost::asio::ip::tcp::resolver::results_type results) {
        ::boost::beast::get_lowest_layer(_stream).async_connect(results,
            ::boost::beast::bind_front_handler(&on_transport_connect<Request, Response>, this, _request, _response));
    };
    template<typename Request, typename Response>
    void on_transport_connect(Request& _request, Response& _response, ::boost::asio::ip::tcp::resolver::results_type::endpoint_type) {
        _stream.async_handshake(::boost::asio::ssl::stream_base::client,
            ::boost::beast::bind_front_handler(&on_ssl_handshake<Request, Response>, this, _request, _response));
    };
    template<typename Request, typename Response>
    void on_ssl_handshake(Request& _request, Response& _response) {
        ::boost::beast::http::async_write(_stream, _request,
            ::boost::beast::bind_front_handler(&on_write<Response>, this, _response));
    };
    template<typename Response>
    void on_write(Response& _response, size_t bytes_transferred) {
        ::boost::ignore_unused(bytes_transferred);
        ::boost::beast::http::async_read(_stream, _buffer, _response,
            ::boost::beast::bind_front_handler(&on_read, this));
    };
    void on_read(size_t bytes_transferred) {
        ::boost::ignore_unused(bytes_transferred);
        _stream.async_shutdown(
            ::boost::beast::bind_front_handler(&on_shutdown, this));
    };
    void on_shutdown(::boost::beast::error_code error_code) {
        if (error_code) {
            if (error_code  == ::boost::asio::ssl::error::stream_truncated) {
                return;
            }
            throw ::boost::beast::system_error{ error_code } ;
        }
    };
};

auth::auth(const ::std::string& address, ::std::chrono::milliseconds timeout)
    : _p_service{ new connection{} } {
    ::boost::system::result<::boost::urls::url_view> result{ ::boost::urls::parse_uri(address) };
    if (!result.has_value()) {
        throw ::std::invalid_argument{ "invalid uri" };
    }
    ::boost::urls::url_view url{ result.value() };
    if (!(url.scheme() == "https")) {
        throw ::std::invalid_argument{ "scheme is not supported" };
    }
    _p_service->_host.append(url.host());
    _p_service->_port.append(url.has_port()
        ? url.port()
        : "443");
    _p_service->_path.append(url.path());
    _p_service->on_init();
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
    ::boost::beast::http::request<::boost::beast::http::empty_body> request{
        ::boost::beast::http::verb::get, "validate", 11,
    };
    request.set(::boost::beast::http::field::authorization, "OAuth " + token);

    ::boost::beast::http::response<::eboost::beast::http::json_body> response{};

    _p_service->on_connect(request, response);
    _p_service->_context.run();
    _p_service->_context.restart();

    return response.result() == ::boost::beast::http::status::ok;
};

token_context auth::refresh_token(const ::std::string& client_id, const ::std::string& client_secret, const ::std::string& refresh_token) {
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

    _p_service->on_connect(request, response);
    _p_service->_context.run();
    _p_service->_context.restart();

    return ::boost::json::value_to<token_context>(response.body());
};

token_context auth::issue_token(const ::std::string& client_id, const ::std::string& device_code, const ::std::string& scopes) {
    ::boost::beast::http::request<::boost::beast::http::empty_body> request{
        ::boost::beast::http::verb::post, "token?grant_type=urn:ietf:params:oauth:grant-type:device_code&client_id=" + client_id + "&device_code=" + device_code + "&scopes=" + scopes, 11,
    };

    ::boost::beast::http::response<::eboost::beast::http::json_body> response{};

    _p_service->on_connect(request, response);
    _p_service->_context.run();
    _p_service->_context.restart();

    return ::boost::json::value_to<token_context>(response.body());
};

access_context auth::request_access(const ::std::string& client_id, const ::std::string& scopes) {
    ::boost::beast::http::request<::boost::beast::http::empty_body> request{
        ::boost::beast::http::verb::post, "device?client_id=" + client_id + "&scopes=" + scopes, 11,
    };

    ::boost::beast::http::response<::eboost::beast::http::json_body> response{};

    _p_service->on_connect(request, response);
    _p_service->_context.run();
    _p_service->_context.restart();

    return ::boost::json::value_to<access_context>(response.body());
};

} // twitch
} // clients
} // chat
} // ai

#endif
