#ifndef TWITCH_AUTH_CLIENT_IPP
#define TWITCH_AUTH_CLIENT_IPP

#include <array>
#include <stdexcept>

#include "boost/url.hpp"

#include "eboost/beast/http/client.hpp"
#include "eboost/beast/http/form_body.hpp"
#include "eboost/beast/http/json_body.hpp"

#include "Twitch/Auth/Client.hpp"

namespace Twitch {
namespace Auth {

Client::Client(const ::std::string& baseAddress, ::std::chrono::steady_clock::duration timeout)
    : m_ssl{}
    , m_host{}
    , m_port{}
    , m_validateTarget{}
    , m_tokenTarget{}
    , m_timeout{ timeout } {
    ::boost::system::result<::boost::urls::url_view> result{ ::boost::urls::parse_uri(baseAddress) };
    if (!result.has_value()) {
        throw ::std::invalid_argument{ "baseAddress" };
    }
    ::boost::urls::url_view url{ result.value() };
    if (url.scheme() == "https") {
        m_ssl = true;
    } else if (!(url.scheme() == "http")) {
        throw ::std::invalid_argument{ "baseAddress" };
    }
    m_host = url.host();
    m_port = url.has_port()
        ? url.port()
        : m_ssl
            ? "443"
            : "80";
    m_validateTarget = url.path() + "validate";
    m_tokenTarget = url.path() + "token";
};

bool Client::ValidateToken(const ::std::string& token) {
    ::boost::beast::http::request<::boost::beast::http::empty_body> request{
        ::boost::beast::http::verb::get, m_validateTarget, 11,
    };
    request.set(::boost::beast::http::field::authorization, "OAuth " + token);

    ::boost::beast::http::response<::eboost::beast::http::json_body> response{
        m_ssl
            ? ::eboost::beast::http::client::send<
                ::eboost::beast::http::client::channel::secure,
                ::boost::beast::http::empty_body,
                ::eboost::beast::http::json_body>(
                    m_host, m_port,
                    m_timeout,
                    request)
            : ::eboost::beast::http::client::send<
                ::eboost::beast::http::client::channel::plain,
                ::boost::beast::http::empty_body,
                ::eboost::beast::http::json_body>(
                    m_host, m_port,
                    m_timeout,
                    request)
    };
    return response.result() == ::boost::beast::http::status::ok;
};

::std::string Client::RefreshToken(const ::std::string& clientId, const ::std::string& clientSecret, const ::std::string& refreshToken) {
    ::boost::beast::http::request<::eboost::beast::http::form_body<::std::array<::std::pair<::std::string, ::std::string>, 4>>> request{
        ::boost::beast::http::verb::post, m_tokenTarget, 11,
        ::std::array<::std::pair<::std::string, ::std::string>, 4>{
            ::std::make_pair("grant_type", "refresh_token"),
            ::std::make_pair("client_id", clientId),
            ::std::make_pair("client_secret", clientSecret),
            ::std::make_pair("refresh_token", refreshToken),
        }
    };
    request.prepare_payload();
    request.set(::boost::beast::http::field::content_type, "application/x-www-form-urlencoded");

    ::boost::beast::http::response<::eboost::beast::http::json_body> response{
        m_ssl
            ? ::eboost::beast::http::client::send<
                ::eboost::beast::http::client::channel::secure,
                ::eboost::beast::http::form_body<::std::array<::std::pair<::std::string, ::std::string>, 4>>,
                ::eboost::beast::http::json_body>(
                    m_host, m_port,
                    m_timeout,
                    request)
            : ::eboost::beast::http::client::send<
                ::eboost::beast::http::client::channel::plain,
                ::eboost::beast::http::form_body<::std::array<::std::pair<::std::string, ::std::string>, 4>>,
                ::eboost::beast::http::json_body>(
                    m_host, m_port,
                    m_timeout,
                    request)
    };
    return ::boost::json::value_to<::std::string>(response.body().at("access_token"));
};

} // Auth
} // Twitch

#endif
