#ifndef AI_CHAT_CLIENTS_AUTH_IPP
#define AI_CHAT_CLIENTS_AUTH_IPP

#include <array>
#include <stdexcept>
#include <utility>

#include "boost/scope/scope_exit.hpp"
#include "re2/re2.h"

#include "ai/chat/clients/auth.hpp"

namespace ai {
namespace chat {
namespace clients {

token_context tag_invoke(::boost::json::value_to_tag<token_context>, ::boost::json::value const &value) {
    return token_context{
        ::boost::json::value_to<::std::string_view>(value.at("access_token")),
        ::boost::json::value_to<::std::string_view>(value.at("refresh_token")),
    };
};
access_context tag_invoke(::boost::json::value_to_tag<access_context>, ::boost::json::value const &value) {
    return access_context{
        ::boost::json::value_to<::std::string_view>(value.at("device_code")),
        ::boost::json::value_to<::std::string_view>(value.at("verification_uri")),
    };
};

auth::auth(::std::string_view address, ::std::chrono::milliseconds timeout)
    : _service{} {
    ::RE2 uri{ R"(https:\/\/(?<host>[^:\/]+)(?::(?<port>\d+))?(?<path>\/.*))" };
    if (!::RE2::FullMatch(address, uri,
            &_service._host, &_service._port, &_service._path)) {
        throw ::std::invalid_argument{ "invalid uri" };
    }
    if (_service._port.empty()) {
        _service._port = "443";
    }
    _service._timeout = timeout;
    _service.on_init();
};

bool auth::validate_token(::std::string_view token) {
    START_SPAN(span, "validate_token", _service)
    _service._request_body.clear();
    _service._h_oauth.resize(::std::size("OAuth ") - 1);
    _service._h_oauth += token;
    _service._request.method(::boost::beast::http::verb::get);
    _service._request.target(_service._t_validate);
    _service._request.version(11);
    _service._request.set(::boost::beast::http::field::host, _service._host);
    _service._request.set(::boost::beast::http::field::authorization, _service._h_oauth);
    auto on_exit = ::boost::scope::make_scope_exit([this]()->void {
        _service.on_reset();
    });
    _service.on_send(
        PROPAGATE_ONLY_SPAN(span));
    _service._io_context.run();
    return _service._response.result() == ::boost::beast::http::status::ok;
};
token_context auth::refresh_token(::std::string_view client_id, ::std::string_view client_secret, ::std::string_view refresh_token) {
    START_SPAN(span, "refresh_token", _service)
    _service._t_token.resize(_service._path.size() + ::std::size("token") - 1);
    _service._request_body.clear();
    _service._request_body += "grant_type=refresh_token";
    _service._request_body += "&client_id="; _service._request_body += client_id;
    _service._request_body += "&client_secret="; _service._request_body += client_secret;
    _service._request_body += "&refresh_token="; _service._request_body += refresh_token;
    _service._request.method(::boost::beast::http::verb::post);
    _service._request.target(_service._t_token);
    _service._request.version(11);
    _service._request.set(::boost::beast::http::field::host, _service._host);
    _service._request.set(::boost::beast::http::field::transfer_encoding, "chunked");
    _service._request.set(::boost::beast::http::field::content_type, "application/x-www-form-urlencoded");
    auto on_exit = ::boost::scope::make_scope_exit([this]()->void {
        _service.on_reset();
    });
    _service.on_send(
        PROPAGATE_ONLY_SPAN(span));
    _service._io_context.run();
    return ::boost::json::value_to<token_context>(_service._response_body);
};
token_context auth::issue_token(::std::string_view client_id, ::std::string_view device_code, ::std::string_view scopes) {
    START_SPAN(span, "issue_token", _service)
    _service._t_token.resize(_service._path.size() + ::std::size("token") - 1);
    _service._t_token += "?grant_type=urn:ietf:params:oauth:grant-type:device_code";
    _service._t_token += "&client_id="; _service._t_token += client_id;
    _service._t_token += "&device_code="; _service._t_token += device_code;
    _service._t_token += "&scopes="; _service._t_token += scopes;
    _service._request_body.clear();
    _service._request.method(::boost::beast::http::verb::post);
    _service._request.target(_service._t_token);
    _service._request.version(11);
    _service._request.set(::boost::beast::http::field::host, _service._host);
    auto on_exit = ::boost::scope::make_scope_exit([this]()->void {
        _service.on_reset();
    });
    _service.on_send(
        PROPAGATE_ONLY_SPAN(span));
    _service._io_context.run();
    return ::boost::json::value_to<token_context>(_service._response_body);
};
access_context auth::request_access(::std::string_view client_id, ::std::string_view scopes) {
    START_SPAN(span, "request_access", _service)
    _service._t_device.resize(_service._path.size() + ::std::size("device") - 1);
    _service._t_device += "?client_id="; _service._t_device += client_id;
    _service._t_device += "&scopes="; _service._t_device += scopes;
    _service._request_body.clear();
    _service._request.method(::boost::beast::http::verb::post);
    _service._request.target(_service._t_device);
    _service._request.version(11);
    _service._request.set(::boost::beast::http::field::host, _service._host);
    auto on_exit = ::boost::scope::make_scope_exit([this]()->void {
        _service.on_reset();
    });
    _service.on_send(
        PROPAGATE_ONLY_SPAN(span));
    _service._io_context.run();
    return ::boost::json::value_to<access_context>(_service._response_body);
};

} // clients
} // chat
} // ai

#endif
