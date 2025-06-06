#ifndef CLIENT_IPP
#define CLIENT_IPP

#include "boost/asio/ssl.hpp"
#include "boost/beast.hpp"
#include "boost/url.hpp"

#include "eboost/beast/json_body.hpp"

#include "OpenAI/Client.hpp"

namespace OpenAI {

Client::Client(const ::std::string& baseAddress, const ::std::string& apiKey)
    : m_host{}
    , m_port{}
    , m_target{}
    , m_ssl{ false }
    , m_authorization{ "Bearer " + apiKey } {
    ::boost::system::result<::boost::urls::url_view> result{ ::boost::urls::parse_uri(baseAddress) };
    ::boost::urls::url_view url{ result.value() };
    if (url.scheme() == "https") {
        m_ssl = true;
    }
    m_host = url.host();
    m_port = url.has_port()
        ? url.port()
        : m_ssl
            ? "443"
            : "80";
    m_target = url.path();
};

void tag_invoke(::boost::json::value_from_tag, ::boost::json::value& value, const Role& role) {
    switch (role) {
        case Role::System:
            value = "system";
            break;
        case Role::User:
            value = "user";
            break;
    }
    value = "assistant";
}
Role tag_invoke(::boost::json::value_to_tag<Role>, const ::boost::json::value& value) {
    const ::boost::json::string& string{ value.as_string() };
    if (string == "system") {
        return Role::System;
    }
    if (string == "user") {
        return Role::User;
    }
    return Role::Assistant;
}

void tag_invoke(::boost::json::value_from_tag, ::boost::json::value& value, const Message& message) {
    value = {
        { "role" , ::boost::json::value_from(message.Role) },
        { "content", message.Content },
    };
}
Message tag_invoke(::boost::json::value_to_tag<Message>, const ::boost::json::value& value) {
    const ::boost::json::object& object{ value.as_object() };
    return {
        ::boost::json::value_to<Role>(value.at("role")),
        ::boost::json::value_to<::std::string>(value.at("content"))
    };
}

template<typename Iterator>
Message Client::Complete(const ::std::string& model, Iterator begin, Iterator end) {
    // The io_context is required for all I/O
    ::boost::asio::io_context ioc;

    // The SSL context is required, and holds certificates
    ::boost::asio::ssl::context ctx(::boost::asio::ssl::context::tlsv12_client);

    // Verify the remote server's certificate
    ctx.set_verify_mode(::boost::asio::ssl::verify_none);

    // These objects perform our I/O
    ::boost::asio::ip::tcp::resolver resolver(ioc);
    ::boost::asio::ssl::stream<::boost::beast::tcp_stream> stream(ioc, ctx);

    // Set SNI Hostname (many hosts need this to handshake successfully)
    if(! SSL_set_tlsext_host_name(stream.native_handle(), m_host.c_str()))
    {
        throw ::boost::beast::system_error(
            static_cast<int>(::ERR_get_error()),
            ::boost::asio::error::get_ssl_category());
    }

    // Set the expected hostname in the peer certificate for verification
    stream.set_verify_callback(::boost::asio::ssl::host_name_verification(m_host));

    // Look up the domain name
    auto const results = resolver.resolve(m_host, m_port);

    // Make the connection on the IP address we get from a lookup
    ::boost::beast::get_lowest_layer(stream).connect(results);

    // Perform the SSL handshake
    stream.handshake(::boost::asio::ssl::stream_base::client);

    // Set up an HTTP GET request message
    ::boost::beast::http::request<::eboost::beast::http::json_body> request{ ::boost::beast::http::verb::post, m_target + "chat/completions", 11 };
    request.set(::boost::beast::http::field::host, m_host);
    request.set(::boost::beast::http::field::user_agent, BOOST_BEAST_VERSION_STRING);
    request.set(::boost::beast::http::field::authorization, m_authorization);
    request.set(::boost::beast::http::field::content_type, "application/json");
    ::boost::json::array messages{};
    for (; begin != end; ++begin) {
        messages.push_back(::boost::json::value_from(*begin));
    }
    request.body() = {
        { "model", model },
        { "messages", messages }
    };
    request.prepare_payload();

    // Send the HTTP request to the remote host
    ::boost::beast::http::write(stream, request);

    // This buffer is used for reading and must be persisted
    ::boost::beast::flat_buffer buffer;

    // Declare a container to hold the response
    ::boost::beast::http::response<::eboost::beast::http::json_body> response;

    // Receive the HTTP response
    ::boost::beast::http::read(stream, buffer, response);

    // Gracefully close the stream
    ::boost::beast::error_code ec;
    stream.shutdown(ec);

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

    if(ec != ::boost::asio::ssl::error::stream_truncated)
        throw ::boost::beast::system_error{ec};

    const ::boost::json::object& object{ response.body().as_object() };
    const ::boost::json::array& choices{ object.at("choices").as_array() };
    const ::boost::json::object& choice{ choices[0].as_object() };

    return ::boost::json::value_to<Message>(choice.at("message"));
};

} // OpenAI

#endif
