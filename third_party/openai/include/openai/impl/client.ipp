#ifndef OPENAI_CLIENT_IPP
#define OPENAI_CLIENT_IPP

#include <stdexcept>

#include "boost/url.hpp"

#include "eboost/beast/http/client.hpp"
#include "eboost/beast/http/json_body.hpp"

#include "openai/client.hpp"

namespace openai {

client::client(const ::std::string& address, const ::std::string& key, ::std::chrono::milliseconds timeout)
    : _ssl{}
    , _host{}
    , _port{}
    , _target_completions{}
    , _authorization{ "Bearer " + key }
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
    _target_completions = url.path() + "chat/completions";
};

::std::string to_string(const role& role) {
    switch (role) {
        case role::system:
            return "system";
        case role::user:
            return "user";
        case role::assistant:
            return "assistant";
    }
    throw ::std::invalid_argument{ "role is not supported" };
};
role tag_invoke(::boost::json::value_to_tag<role>, const ::boost::json::value& value) {
    const ::boost::json::string& string{ value.as_string() };
    if (string == "system") {
        return role::system;
    }
    if (string == "user") {
        return role::user;
    }
    if (string == "assistant") {
        return role::assistant;
    }
    throw ::std::invalid_argument{ "role is not supported" };
};

void tag_invoke(::boost::json::value_from_tag, ::boost::json::value& value, const message& message) {
    value = {
        { "role" , to_string(message.role) },
        { "content", message.content },
    };
};
message tag_invoke(::boost::json::value_to_tag<message>, const ::boost::json::value& value) {
    return {
        ::boost::json::value_to<role>(value.at("role")),
        ::boost::json::value_to<::std::string>(value.at("content")),
    };
};

usage tag_invoke(::boost::json::value_to_tag<usage>, const ::boost::json::value& value) {
    return {
        ::boost::json::value_to<size_t>(value.at("completion_tokens")),
        ::boost::json::value_to<size_t>(value.at("prompt_tokens")),
        ::boost::json::value_to<size_t>(value.at("total_tokens")),
    };
};

finish_reason tag_invoke(::boost::json::value_to_tag<finish_reason>, const ::boost::json::value& value) {
    const ::boost::json::string& string{ value.as_string() };
    if (string == "stop") {
        return finish_reason::stop;
    }
    if (string == "length") {
        return finish_reason::length;
    }
    if (string == "content_filter") {
        return finish_reason::content_filter;
    }
    if (string == "tool_calls") {
        return finish_reason::tool_calls;
    }
    throw ::std::invalid_argument{ "finish reason is not supported" };
};

choice tag_invoke(::boost::json::value_to_tag<choice>, const ::boost::json::value& value) {
    return {
        ::boost::json::value_to<finish_reason>(value.at("finish_reason")),
        ::boost::json::value_to<size_t>(value.at("index")),
        ::boost::json::value_to<message>(value.at("message")),
    };
};

completion_result tag_invoke(::boost::json::value_to_tag<completion_result>, const ::boost::json::value& value) {
    return {
        ::boost::json::value_to<::std::vector<choice>>(value.at("choices")),
        ::boost::json::value_to<usage>(value.at("usage")),
    };
};

template<typename Range>
void tag_invoke(::boost::json::value_from_tag, ::boost::json::value& value, const completion_context<Range>& context) {
    value = {
        { "model" , context.model },
        { "messages", ::boost::json::value_from(context.messages) },
    };
};

template<typename Range>
completion_result client::complete(const completion_context<Range>& context) const {
    ::boost::beast::http::request<::eboost::beast::http::json_body> request{
        ::boost::beast::http::verb::post, _target_completions, 11,
        ::boost::json::value_from(context)
    };
    request.prepare_payload();
    request.set(::boost::beast::http::field::authorization, _authorization);
    request.set(::boost::beast::http::field::content_type, "application/json");

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
    return ::boost::json::value_to<completion_result>(response.body());
};

} // openai

#endif
