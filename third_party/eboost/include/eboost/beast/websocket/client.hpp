#ifndef EBOOST_BEAST_WEBSOCKET_CLIENT_HPP
#define EBOOST_BEAST_WEBSOCKET_CLIENT_HPP

#include <chrono>
#include <string>

#include <boost/asio/ssl.hpp>
#include <boost/asio/strand.hpp>
#include <boost/beast.hpp>
#include <boost/beast/ssl.hpp>

#include "eboost/beast/channel.hpp"
#include "eboost/beast/ensure_success.hpp"

namespace eboost {
namespace beast {
namespace websocket {
namespace client {

template<typename Callback>
void run(::eboost::beast::secure_channel_tag, const ::std::string& host, const ::std::string& port, ::std::chrono::milliseconds timeout, Callback&& callback) {
    ::boost::asio::io_context io_context{};

    ::boost::asio::ip::tcp::resolver resolver{ ::boost::asio::make_strand(io_context) };

    ::boost::asio::ssl::context ssl_context{ ::boost::asio::ssl::context::tlsv12_client };
    ssl_context.set_verify_mode(::boost::asio::ssl::verify_none);

    ::boost::beast::websocket::stream<::boost::asio::ssl::stream<::boost::beast::tcp_stream>> stream{ ::boost::asio::make_strand(io_context), ssl_context };
    if (!::SSL_set_tlsext_host_name(stream.next_layer().native_handle(), host.c_str())) {
        throw ::boost::beast::system_error{
            static_cast<int>(::ERR_get_error()),
            ::boost::asio::error::get_ssl_category()
        };
    }
    stream.next_layer().set_verify_callback(::boost::asio::ssl::host_name_verification(host));

    // Set a timeout on the operation
    ::boost::beast::get_lowest_layer(stream).expires_after(timeout);

     // Look up the domain name
    resolver.async_resolve(host, port, [&](::boost::beast::error_code error_code, ::boost::asio::ip::tcp::resolver::results_type results)->void {
    ::eboost::beast::ensure_success(error_code);

    // Make the connection on the IP address we get from a lookup
    ::boost::beast::get_lowest_layer(stream).async_connect(results, [&](::boost::beast::error_code error_code, ::boost::asio::ip::tcp::resolver::results_type::endpoint_type)->void {
    ::eboost::beast::ensure_success(error_code);

    // Perform the SSL handshake
    stream.next_layer().async_handshake(::boost::asio::ssl::stream_base::client, [&](::boost::beast::error_code error_code)->void {
    ::eboost::beast::ensure_success(error_code);

    ::boost::beast::get_lowest_layer(stream).expires_never();
    stream.set_option(
        ::boost::beast::websocket::stream_base::timeout::suggested(
            ::boost::beast::role_type::client));

    stream.async_handshake(host + ":" + port, "/", [&](::boost::beast::error_code error_code)->void {
    ::eboost::beast::ensure_success(error_code);

    callback(stream);

    }); // WebSocket handshake
    }); // SSL handshake
    }); // connect
    }); // resolve

    io_context.run();
};

template<typename Callback>
void run(::eboost::beast::plain_channel_tag, const ::std::string& host, const ::std::string& port, ::std::chrono::milliseconds timeout, Callback&& callback) {
    ::boost::asio::io_context io_context{};

    ::boost::asio::ip::tcp::resolver resolver{ ::boost::asio::make_strand(io_context) };

    ::boost::beast::websocket::stream<::boost::beast::tcp_stream> stream{ ::boost::asio::make_strand(io_context) };

    // Set a timeout on the operation
    ::boost::beast::get_lowest_layer(stream).expires_after(timeout);

     // Look up the domain name
    resolver.async_resolve(host, port, [&](::boost::beast::error_code error_code, ::boost::asio::ip::tcp::resolver::results_type results)->void {
    ::eboost::beast::ensure_success(error_code);

    // Make the connection on the IP address we get from a lookup
    ::boost::beast::get_lowest_layer(stream).async_connect(results, [&](::boost::beast::error_code error_code, ::boost::asio::ip::tcp::resolver::results_type::endpoint_type)->void {
    ::eboost::beast::ensure_success(error_code);

    ::boost::beast::get_lowest_layer(stream).expires_never();
    stream.set_option(
        ::boost::beast::websocket::stream_base::timeout::suggested(
            ::boost::beast::role_type::client));

    stream.async_handshake(host + ":" + port, "/", [&](::boost::beast::error_code error_code)->void {
    ::eboost::beast::ensure_success(error_code);

    callback(stream);

    }); // WebSocket handshake
    }); // connect
    }); // resolve

    io_context.run();
};

} // client
} // websocket
} // beast
} // eboost

#endif
