#ifndef TWITCH_AUTH_CLIENT_IPP
#define TWITCH_AUTH_CLIENT_IPP

#include <array>
#include <stdexcept>

#include "boost/url.hpp"

#include "eboost/beast/http/client.hpp"
#include "eboost/beast/http/form_body.hpp"
#include "eboost/beast/http/json_body.hpp"

#include "twitch/auth/client.hpp"

namespace twitch {
namespace auth {

client::client(const ::std::string& address, ::std::chrono::milliseconds timeout)
    : _ssl{}
    , _host{}
    , _port{}
    , _target_validate{}
    , _target_token{}
    , _target_device{}
    , _timeout{ timeout } {
    ::boost::system::result<::boost::urls::url_view> result{ ::boost::urls::parse_uri(address) };
    if (!result.has_value()) {
        throw ::std::invalid_argument{ "invalid uri" };
    }
    ::boost::urls::url_view url{ result.value() };
    if (url.scheme() == "https") {
        _ssl = true;
    } else if (!(url.scheme() == "http")) {
        throw ::std::invalid_argument{ "scheme is not supported" };
    }
    _host = url.host();
    _port = url.has_port()
        ? url.port()
        : _ssl
            ? "443"
            : "80";
    _target_validate = url.path() + "validate";
    _target_token = url.path() + "token";
    _target_device = url.path() + "device";
};

access_context tag_invoke(::boost::json::value_to_tag<access_context>, const ::boost::json::value& value) {
    return {
        ::boost::json::value_to<::std::string>(value.at("access_token")),
        ::boost::json::value_to<::std::string>(value.at("refresh_token")),
    };
};
auth_context tag_invoke(::boost::json::value_to_tag<auth_context>, const ::boost::json::value& value) {
    return {
        ::boost::json::value_to<::std::string>(value.at("device_code")),
        ::boost::json::value_to<::std::string>(value.at("verification_uri")),
    };
};

bool client::validate_token(const ::std::string& token) const {
    ::boost::beast::http::request<::boost::beast::http::empty_body> request{
        ::boost::beast::http::verb::get, _target_validate, 11,
    };
    request.set(::boost::beast::http::field::authorization, "OAuth " + token);

    ::boost::beast::http::response<::eboost::beast::http::json_body> response{
        _ssl
            ? ::eboost::beast::http::client::send<::eboost::beast::http::json_body>(
                ::eboost::beast::secure_channel_tag{},
                _host, _port,
                _timeout,
                request)
            : ::eboost::beast::http::client::send<::eboost::beast::http::json_body>(
                ::eboost::beast::plain_channel_tag{},
                _host, _port,
                _timeout,
                request)
    };
    return response.result() == ::boost::beast::http::status::ok;
};

access_context client::refresh_token(const ::std::string& client_id, const ::std::string& client_secret, const ::std::string& refresh_token) const {
    ::boost::beast::http::request<::eboost::beast::http::form_body<::std::array<::std::pair<::std::string, ::std::string>, 4>>> request{
        ::boost::beast::http::verb::post, _target_token, 11,
        ::std::array<::std::pair<::std::string, ::std::string>, 4>{
            ::std::make_pair("grant_type", "refresh_token"),
            ::std::make_pair("client_id", client_id),
            ::std::make_pair("client_secret", client_secret),
            ::std::make_pair("refresh_token", refresh_token),
        }
    };
    request.prepare_payload();
    request.set(::boost::beast::http::field::content_type, "application/x-www-form-urlencoded");

    ::boost::beast::http::response<::eboost::beast::http::json_body> response{
        _ssl
            ? ::eboost::beast::http::client::send<::eboost::beast::http::json_body>(
                ::eboost::beast::secure_channel_tag{},
                _host, _port,
                _timeout,
                request)
            : ::eboost::beast::http::client::send<::eboost::beast::http::json_body>(
                ::eboost::beast::plain_channel_tag{},
                _host, _port,
                _timeout,
                request)
    };
    return ::boost::json::value_to<access_context>(response.body());
};

access_context client::issue_token(const ::std::string& client_id, const ::std::string& device_code, const ::std::string& scopes) const {
    ::boost::beast::http::request<::boost::beast::http::empty_body> request{
        ::boost::beast::http::verb::post, _target_token + "?client_id=" + client_id + "&grant_type=urn:ietf:params:oauth:grant-type:device_code&device_code=" + device_code + "&scopes=" + scopes, 11,
    };

    ::boost::beast::http::response<::eboost::beast::http::json_body> response{
        _ssl
            ? ::eboost::beast::http::client::send<::eboost::beast::http::json_body>(
                ::eboost::beast::secure_channel_tag{},
                _host, _port,
                _timeout,
                request)
            : ::eboost::beast::http::client::send<::eboost::beast::http::json_body>(
                ::eboost::beast::plain_channel_tag{},
                _host, _port,
                _timeout,
                request)
    };
    return ::boost::json::value_to<access_context>(response.body());
};

auth_context client::request_access(const ::std::string& client_id, const ::std::string& scopes) const {
    ::boost::beast::http::request<::boost::beast::http::empty_body> request{
        ::boost::beast::http::verb::post, _target_device + "?client_id=" + client_id + "&scopes=" + scopes, 11,
    };

    ::boost::beast::http::response<::eboost::beast::http::json_body> response{
        _ssl
            ? ::eboost::beast::http::client::send<::eboost::beast::http::json_body>(
                ::eboost::beast::secure_channel_tag{},
                _host, _port,
                _timeout,
                request)
            : ::eboost::beast::http::client::send<::eboost::beast::http::json_body>(
                ::eboost::beast::plain_channel_tag{},
                _host, _port,
                _timeout,
                request)
    };
    return ::boost::json::value_to<auth_context>(response.body());
};

} // auth
} // twitch

#endif
