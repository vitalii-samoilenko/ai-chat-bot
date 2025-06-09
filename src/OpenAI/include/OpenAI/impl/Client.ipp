#ifndef OPENAI_CLIENT_IPP
#define OPENAI_CLIENT_IPP

#include <stdexcept>

#include "boost/url.hpp"

#include "eboost/beast/http/client.hpp"
#include "eboost/beast/http/json_body.hpp"

#include "OpenAI/Client.hpp"

namespace OpenAI {

Client::Client(const ::std::string& baseAddress, const ::std::string& apiKey, ::std::chrono::milliseconds timeout)
    : m_ssl{}
    , m_host{}
    , m_port{}
    , m_completionsTarget{}
    , m_authorization{ "Bearer " + apiKey }
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
    m_completionsTarget = url.path() + "chat/completions";
};

::std::string to_string(const Role& role) {
    switch (role) {
        case Role::System:
            return "system";
        case Role::User:
            return "user";
        case Role::Assistant:
            return "assistant";
    }
    throw ::std::invalid_argument{ "role" };
};
Role tag_invoke(::boost::json::value_to_tag<Role>, const ::boost::json::value& value) {
    const ::boost::json::string& string{ value.as_string() };
    if (string == "system") {
        return Role::System;
    }
    if (string == "user") {
        return Role::User;
    }
    if (string == "assistant") {
        return Role::Assistant;
    }
    throw ::std::invalid_argument{ "value" };
};

void tag_invoke(::boost::json::value_from_tag, ::boost::json::value& value, const Message& message) {
    value = {
        { "role" , to_string(message.Role) },
        { "content", message.Content },
    };
};
Message tag_invoke(::boost::json::value_to_tag<Message>, const ::boost::json::value& value) {
    return {
        ::boost::json::value_to<Role>(value.at("role")),
        ::boost::json::value_to<::std::string>(value.at("content")),
    };
};

Usage tag_invoke(::boost::json::value_to_tag<Usage>, const ::boost::json::value& value) {
    return {
        ::boost::json::value_to<size_t>(value.at("completion_tokens")),
        ::boost::json::value_to<size_t>(value.at("prompt_tokens")),
        ::boost::json::value_to<size_t>(value.at("total_tokens")),
    };
};

FinishReason tag_invoke(::boost::json::value_to_tag<FinishReason>, const ::boost::json::value& value) {
    const ::boost::json::string& string{ value.as_string() };
    if (string == "stop") {
        return FinishReason::Stop;
    }
    if (string == "length") {
        return FinishReason::Length;
    }
    if (string == "content_filter") {
        return FinishReason::ContentFilter;
    }
    if (string == "tool_calls") {
        return FinishReason::ToolCalls;
    }
    throw ::std::invalid_argument{ "value" };
};

Choice tag_invoke(::boost::json::value_to_tag<Choice>, const ::boost::json::value& value) {
    return {
        ::boost::json::value_to<FinishReason>(value.at("finish_reason")),
        ::boost::json::value_to<size_t>(value.at("index")),
        ::boost::json::value_to<Message>(value.at("message")),
    };
};

CompletionResult tag_invoke(::boost::json::value_to_tag<CompletionResult>, const ::boost::json::value& value) {
    return {
        ::boost::json::value_to<::std::vector<Choice>>(value.at("choices")),
        ::boost::json::value_to<Usage>(value.at("usage")),
    };
};

template<typename Range>
void tag_invoke(::boost::json::value_from_tag, ::boost::json::value& value, const CompletionContext<Range>& context) {
    value = {
        { "model" , context.Model },
        { "messages", ::boost::json::value_from(context.Messages) },
    };
};

template<typename Range>
CompletionResult Client::Complete(const CompletionContext<Range>& context) const {
    ::boost::beast::http::request<::eboost::beast::http::json_body> request{
        ::boost::beast::http::verb::post, m_completionsTarget, 11,
        ::boost::json::value_from(context)
    };
    request.prepare_payload();
    request.set(::boost::beast::http::field::authorization, m_authorization);
    request.set(::boost::beast::http::field::content_type, "application/json");

    ::boost::beast::http::response<::eboost::beast::http::json_body> response{
        ::eboost::beast::http::client::send<::eboost::beast::http::json_body>(
            m_ssl, m_host, m_port,
            m_timeout,
            request)
    };
    return ::boost::json::value_to<CompletionResult>(response.body());
};

} // OpenAI

#endif
