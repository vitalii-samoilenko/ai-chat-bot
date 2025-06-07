#ifndef CLIENT_IPP
#define CLIENT_IPP

#include <stdexcept>

#include "boost/asio/ssl.hpp"
#include "boost/asio/strand.hpp"
#include "boost/beast.hpp"
#include "boost/url.hpp"

#include "eboost/beast/http/json_body.hpp"

#include "OpenAI/Client.hpp"

namespace OpenAI {

template<>
Client<Type::Secure>::Client(const ::std::string& baseAddress, const ::std::string& apiKey, ::std::chrono::steady_clock::duration timeout)
    : m_host{}
    , m_port{}
    , m_completionsTarget{}
    , m_authorization{ "Bearer " + apiKey }
    , m_timeout{ timeout } {
    ::boost::system::result<::boost::urls::url_view> result{ ::boost::urls::parse_uri(baseAddress) };
    if (!result.has_value()) {
        throw ::std::invalid_argument{ "baseAddress" };
    }
    ::boost::urls::url_view url{ result.value() };
    if (url.scheme() != "https") {
        throw ::std::invalid_argument{ "baseAddress" };
    }
    m_host = url.host();
    m_port = url.has_port()
        ? url.port()
        : "443";
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
}
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
}

void tag_invoke(::boost::json::value_from_tag, ::boost::json::value& value, const Message& message) {
    value = {
        { "role" , to_string(message.Role) },
        { "content", message.Content },
    };
}
Message tag_invoke(::boost::json::value_to_tag<Message>, const ::boost::json::value& value) {
    return {
        ::boost::json::value_to<Role>(value.at("role")),
        ::boost::json::value_to<::std::string>(value.at("content")),
    };
}

Usage tag_invoke(::boost::json::value_to_tag<Usage>, const ::boost::json::value& value) {
    return {
        ::boost::json::value_to<size_t>(value.at("completion_tokens")),
        ::boost::json::value_to<size_t>(value.at("prompt_tokens")),
        ::boost::json::value_to<size_t>(value.at("total_tokens")),
    };
}

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
}

Choice tag_invoke(::boost::json::value_to_tag<Choice>, const ::boost::json::value& value) {
    return {
        ::boost::json::value_to<FinishReason>(value.at("finish_reason")),
        ::boost::json::value_to<size_t>(value.at("index")),
        ::boost::json::value_to<Message>(value.at("message")),
    };
}

CompletionResult tag_invoke(::boost::json::value_to_tag<CompletionResult>, const ::boost::json::value& value) {
    return {
        ::boost::json::value_to<::std::vector<Choice>>(value.at("choices")),
        ::boost::json::value_to<Usage>(value.at("usage")),
    };
}

template<typename Range>
void tag_invoke(::boost::json::value_from_tag, ::boost::json::value& value, const CompletionContext<Range>& context) {
    value = {
        { "model" , context.Model },
        { "messages", ::boost::json::value_from(context.Messages) },
    };
}

void ensureSuccess(::boost::beast::error_code errorCode) {
    if (errorCode) {
        throw ::boost::beast::system_error{ errorCode } ;
    }
}

template<>
template<typename Range>
CompletionResult Client<Type::Secure>::Complete(const CompletionContext<Range>& context) {
    ::boost::asio::io_context ioContext{};

    ::boost::asio::ip::tcp::resolver resolver{ ::boost::asio::make_strand(ioContext) };

    ::boost::asio::ssl::context sslContext{ ::boost::asio::ssl::context::tlsv12_client };
    sslContext.set_verify_mode(::boost::asio::ssl::verify_none);

    ::boost::asio::ssl::stream<::boost::beast::tcp_stream> stream{ ::boost::asio::make_strand(ioContext), sslContext };
    if (!::SSL_set_tlsext_host_name(stream.native_handle(), m_host.c_str())) {
        throw ::boost::beast::system_error{
            static_cast<int>(::ERR_get_error()),
            ::boost::asio::error::get_ssl_category()
        };
    }
    stream.set_verify_callback(::boost::asio::ssl::host_name_verification(m_host));

    ::boost::beast::http::request<::eboost::beast::http::json_body> request{ ::boost::beast::http::verb::post, m_completionsTarget, 11 };
    request.set(::boost::beast::http::field::host, m_host);
    request.set(::boost::beast::http::field::user_agent, BOOST_BEAST_VERSION_STRING);
    request.set(::boost::beast::http::field::authorization, m_authorization);
    request.set(::boost::beast::http::field::content_type, "application/json");
    request.body() = ::boost::json::value_from(context);
    request.prepare_payload();

    ::boost::beast::flat_buffer buffer{};

    ::boost::beast::http::response<::eboost::beast::http::json_body> response{};

    // Set a timeout on the operation
    ::boost::beast::get_lowest_layer(stream).expires_after(m_timeout);

     // Look up the domain name
    resolver.async_resolve(m_host, m_port, [&](::boost::beast::error_code errorCode, ::boost::asio::ip::tcp::resolver::results_type results)->void {
    ensureSuccess(errorCode);

    // Make the connection on the IP address we get from a lookup
    ::boost::beast::get_lowest_layer(stream).async_connect(results, [&](::boost::beast::error_code errorCode, ::boost::asio::ip::tcp::resolver::results_type::endpoint_type)->void {
    ensureSuccess(errorCode);

    // Perform the SSL handshake
    stream.async_handshake(::boost::asio::ssl::stream_base::client, [&](::boost::beast::error_code errorCode)->void {
    ensureSuccess(errorCode);

    // Send the HTTP request to the remote host
    ::boost::beast::http::async_write(stream, request, [&](::boost::beast::error_code errorCode, size_t transferredBytes)->void {
    ::boost::ignore_unused(transferredBytes);
    ensureSuccess(errorCode);

    // Receive the HTTP response
    ::boost::beast::http::async_read(stream, buffer, response, [&](::boost::beast::error_code errorCode, size_t transferredBytes)->void {
    ::boost::ignore_unused(transferredBytes);
    ensureSuccess(errorCode);

    // Gracefully close the stream
    stream.async_shutdown([&](::boost::beast::error_code errorCode)->void{
    // ssl::error::stream_truncated, also known as an SSL "short read",
    // indicates the peer closed the connection without performing the
    // required closing handshake (for example, Google does this to
    // improve performance). Generally this can be a security issue,
    // but if your communication protocol is self-terminated (as
    // it is with both HTTP and WebSocket) then you may simply
    // ignore the lack of close_notify.
    //
    // https://github.com/boostorg/beast/issues/38
    //
    // https://security.stackexchange.com/questions/91435/how-to-handle-a-malicious-ssl-tls-shutdown
    //
    // When a short read would cut off the end of an HTTP message,
    // Beast returns the error beast::http::error::partial_message.
    // Therefore, if we see a short read here, it has occurred
    // after the message has been completed, so it is safe to ignore it.
    //
    // not_connected happens sometimes so don't bother reporting it.

    if (errorCode == ::boost::asio::ssl::error::stream_truncated) {
        return;
    }
    ensureSuccess(errorCode);

    // If we get here then the connection is closed gracefully
    }); // shutdown
    }); // read
    }); // write
    }); // handshake
    }); // connect
    }); // resolve

    ioContext.run();

    return ::boost::json::value_to<CompletionResult>(response.body());
};

} // OpenAI

#endif
