#ifndef EBOOST_BEAST_HTTP_CLIENT_HPP
#define EBOOST_BEAST_HTTP_CLIENT_HPP

#include <chrono>
#include <string>

#include <boost/asio/ssl.hpp>
#include <boost/asio/strand.hpp>
#include <boost/beast.hpp>

#include "eboost/beast/channel.hpp"
#include "eboost/beast/ensure_success.hpp"

namespace eboost {
namespace beast {
namespace http {
namespace client {

template<typename ResponseBody, typename RequestBody>
::boost::beast::http::response<ResponseBody> send(::eboost::beast::secure_channel_tag, const ::std::string& host, const ::std::string& port, ::std::chrono::milliseconds timeout, ::boost::beast::http::request<RequestBody>& request) {
    ::boost::asio::io_context ioContext{};

    ::boost::asio::ip::tcp::resolver resolver{ ::boost::asio::make_strand(ioContext) };

    ::boost::asio::ssl::context sslContext{ ::boost::asio::ssl::context::tlsv12_client };
    sslContext.set_verify_mode(::boost::asio::ssl::verify_none);

    ::boost::asio::ssl::stream<::boost::beast::tcp_stream> stream{ ::boost::asio::make_strand(ioContext), sslContext };
    if (!::SSL_set_tlsext_host_name(stream.native_handle(), host.c_str())) {
        throw ::boost::beast::system_error{
            static_cast<int>(::ERR_get_error()),
            ::boost::asio::error::get_ssl_category()
        };
    }
    stream.set_verify_callback(::boost::asio::ssl::host_name_verification(host));

    request.set(::boost::beast::http::field::host, host);
    request.set(::boost::beast::http::field::user_agent, BOOST_BEAST_VERSION_STRING);

    ::boost::beast::flat_buffer buffer{};

    ::boost::beast::http::response<ResponseBody> response{};

    // Set a timeout on the operation
    ::boost::beast::get_lowest_layer(stream).expires_after(timeout);

     // Look up the domain name
    resolver.async_resolve(host, port, [&](::boost::beast::error_code errorCode, ::boost::asio::ip::tcp::resolver::results_type results)->void {
    ::eboost::beast::ensure_success(errorCode);

    // Make the connection on the IP address we get from a lookup
    ::boost::beast::get_lowest_layer(stream).async_connect(results, [&](::boost::beast::error_code errorCode, ::boost::asio::ip::tcp::resolver::results_type::endpoint_type)->void {
    ::eboost::beast::ensure_success(errorCode);

    // Perform the SSL handshake
    stream.async_handshake(::boost::asio::ssl::stream_base::client, [&](::boost::beast::error_code errorCode)->void {
    ::eboost::beast::ensure_success(errorCode);

    // Send the HTTP request to the remote host
    ::boost::beast::http::async_write(stream, request, [&](::boost::beast::error_code errorCode, size_t transferredBytes)->void {
    ::boost::ignore_unused(transferredBytes);
    ::eboost::beast::ensure_success(errorCode);

    // Receive the HTTP response
    ::boost::beast::http::async_read(stream, buffer, response, [&](::boost::beast::error_code errorCode, size_t transferredBytes)->void {
    ::boost::ignore_unused(transferredBytes);
    ::eboost::beast::ensure_success(errorCode);

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
    ::eboost::beast::ensure_success(errorCode);

    // If we get here then the connection is closed gracefully

    }); // shutdown
    }); // read
    }); // write
    }); // handshake
    }); // connect
    }); // resolve

    ioContext.run();

    return response;
}

template<typename ResponseBody, typename RequestBody>
::boost::beast::http::response<ResponseBody> send(::eboost::beast::plain_channel_tag, const ::std::string& host, const ::std::string& port, ::std::chrono::milliseconds timeout, ::boost::beast::http::request<RequestBody>& request) {
    ::boost::asio::io_context ioContext{};

    ::boost::asio::ip::tcp::resolver resolver{ ::boost::asio::make_strand(ioContext) };

    ::boost::beast::tcp_stream stream{ ::boost::asio::make_strand(ioContext) };

    request.set(::boost::beast::http::field::host, host);
    request.set(::boost::beast::http::field::user_agent, BOOST_BEAST_VERSION_STRING);

    ::boost::beast::flat_buffer buffer{};

    ::boost::beast::http::response<ResponseBody> response{};

    // Set a timeout on the operation
    stream.expires_after(timeout);

     // Look up the domain name
    resolver.async_resolve(host, port, [&](::boost::beast::error_code errorCode, ::boost::asio::ip::tcp::resolver::results_type results)->void {
    ::eboost::beast::ensure_success(errorCode);

    // Make the connection on the IP address we get from a lookup
    stream.async_connect(results, [&](::boost::beast::error_code errorCode, ::boost::asio::ip::tcp::resolver::results_type::endpoint_type)->void {
    ::eboost::beast::ensure_success(errorCode);

    // Send the HTTP request to the remote host
    ::boost::beast::http::async_write(stream, request, [&](::boost::beast::error_code errorCode, size_t transferredBytes)->void {
    ::boost::ignore_unused(transferredBytes);
    ::eboost::beast::ensure_success(errorCode);

    // Receive the HTTP response
    ::boost::beast::http::async_read(stream, buffer, response, [&](::boost::beast::error_code errorCode, size_t transferredBytes)->void {
    ::boost::ignore_unused(transferredBytes);
    ::eboost::beast::ensure_success(errorCode);

    // Gracefully close the socket
    stream.socket().shutdown(::boost::asio::ip::tcp::socket::shutdown_both, errorCode);

    // not_connected happens sometimes so don't bother reporting it.
    if (errorCode == ::boost::beast::errc::not_connected) {
        return;
    }
    ::eboost::beast::ensure_success(errorCode);

    // If we get here then the connection is closed gracefully

    }); // read
    }); // write
    }); // connect
    }); // resolve

    ioContext.run();

    return response;
}

} // client
} // http
} // beast
} // eboost

#endif
