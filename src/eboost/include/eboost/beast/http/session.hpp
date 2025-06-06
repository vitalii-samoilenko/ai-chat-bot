//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/boostorg/beast
//

//------------------------------------------------------------------------------
//
// Example: HTTP client, asynchronous
//
//------------------------------------------------------------------------------

#ifndef EBOOST_BEAST_HTTP_CLIENT_HPP
#define EBOOST_BEAST_HTTP_CLIENT_HPP

#include <memory>
#include <string>
#include <utility>

#include <boost/asio/ssl.hpp>
#include <boost/asio/strand.hpp>
#include <boost/beast.hpp>

namespace eboost {
namespace beast {
namespace http {

// Performs an HTTP GET and prints the response
template<typename TRequestBody, typename TResponseBody>
class session : public ::std::enable_shared_from_this<session<TRequestBody, TResponseBody>> {
public:
    session() = delete;
    session(const session&) = delete;
    session(session&&) = delete;

    ~session() = default;

    session& operator=(const session&) = delete;
    session& operator=(session&&) = delete;

    // Objects are constructed with a strand to
    // ensure that handlers do not execute concurrently.
    session(bool ssl, ::std::chrono::steady_clock::duration timeout)
        : m_ioContext{}
        , m_sslContext{ ::boost::asio::ssl::context::tlsv12_client }
        , m_resolver{ ::boost::asio::make_strand(m_ioContext) }
        , m_stream{ ::boost::asio::make_strand(m_ioContext), m_sslContext }
        , m_buffer{}
        , m_request{}
        , m_response{}
        , m_errorCode{}
        , m_ssl{ ssl }
        , m_timeout{ timeout } {
        if (m_ssl) {
            m_sslContext.set_verify_mode(::boost::asio::ssl::verify_none);
        }
    }

    // Start the asynchronous operation
    void run(const ::std::string& host, const ::std::string& port) {
        if (m_ssl) {
            if (!::SSL_set_tlsext_host_name(m_stream.native_handle(), host.c_str())) {
                throw ::boost::beast::system_error{
                    static_cast<int>(::ERR_get_error()),
                    ::boost::asio::error::get_ssl_category()
                };
            }
            m_stream.set_verify_callback(::boost::asio::ssl::host_name_verification(host));
        }

        // Look up the domain name
        m_resolver.async_resolve(host, port,
            ::boost::beast::bind_front_handler(
                &session::on_resolve,
                this->shared_from_this()));
    }
    void wait() {
        m_ioContext.run();
        if (m_errorCode) {
            throw ::boost::beast::system_error{ m_errorCode } ;
        }
    }

    ::boost::beast::http::request<TRequestBody>& request() {
        return m_request;
    }
    ::boost::beast::http::response<TResponseBody>& response() {
        return m_response;
    }

private:
    ::boost::asio::io_context m_ioContext;
    ::boost::asio::ssl::context m_sslContext;
    ::boost::asio::ip::tcp::resolver m_resolver;
    ::boost::asio::ssl::stream<::boost::beast::tcp_stream> m_stream;
    ::boost::beast::flat_buffer m_buffer;
    ::boost::beast::http::request<TRequestBody> m_request;
    ::boost::beast::http::response<TResponseBody> m_response;
    ::boost::beast::error_code m_errorCode;
    bool m_ssl;
    ::std::chrono::steady_clock::duration m_timeout;

    void on_resolve(::boost::beast::error_code errorCode, ::boost::asio::ip::tcp::resolver::results_type results) {
        if (errorCode) {
            m_errorCode = errorCode;
            return;
        }

        // Set a timeout on the operation
        ::boost::beast::get_lowest_layer(m_stream).expires_after(m_timeout);

        // Make the connection on the IP address we get from a lookup
        ::boost::beast::get_lowest_layer(m_stream).async_connect(results,
            ::boost::beast::bind_front_handler(
                &session::on_connect,
                this->shared_from_this()));
    }

    void on_connect(::boost::beast::error_code errorCode, ::boost::asio::ip::tcp::resolver::results_type::endpoint_type) {
        if (!m_ssl) {
            on_handshake(&::boost::beast::get_lowest_layer(m_stream), errorCode);
            return;
        }

        if (errorCode) {
            m_errorCode = errorCode;
            return;
        }

        // Perform the SSL handshake
        m_stream.async_handshake(::boost::asio::ssl::stream_base::client,
            ::boost::beast::bind_front_handler(
                &session::on_handshake<::boost::asio::ssl::stream<::boost::beast::tcp_stream>>,
                this->shared_from_this(),
                &m_stream));
    }

    template<typename Stream>
    void on_handshake(Stream* pStream, ::boost::beast::error_code errorCode) {
        if (errorCode) {
            m_errorCode = errorCode;
            return;
        }

        // Set a timeout on the operation
        ::boost::beast::get_lowest_layer(*pStream).expires_after(m_timeout);

        // Send the HTTP request to the remote host
        ::boost::beast::http::async_write(*pStream, m_request,
            ::boost::beast::bind_front_handler(
                &session::on_write<Stream>,
                this->shared_from_this(),
                pStream));
    }

    template<typename Stream>
    void on_write(Stream* pStream, ::boost::beast::error_code errorCode, size_t transferredBytes) {
        ::boost::ignore_unused(transferredBytes);

        if (errorCode) {
            m_errorCode = errorCode;
            return;
        }
        
        // Receive the HTTP response
        ::boost::beast::http::async_read(*pStream, m_buffer, m_response,
            ::boost::beast::bind_front_handler(
                &session::on_read,
                this->shared_from_this()));
    }

    void on_read(::boost::beast::error_code errorCode, size_t transferredBytes) {
        ::boost::ignore_unused(transferredBytes);

        if (errorCode) {
            m_errorCode = errorCode;
            return;
        } 

        if (!m_ssl) {
            ::boost::beast::get_lowest_layer(m_stream).socket().shutdown(::boost::asio::ip::tcp::socket::shutdown_both, errorCode);
            on_shutdown(errorCode);
            return;
        }

        // Set a timeout on the operation
        ::boost::beast::get_lowest_layer(m_stream).expires_after(m_timeout);

        // Gracefully close the stream
        m_stream.async_shutdown(
            ::boost::beast::bind_front_handler(
                &session::on_shutdown,
                this->shared_from_this()));
    }

    void on_shutdown(::boost::beast::error_code errorCode) {
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

        if(errorCode) {
            if (errorCode == ::boost::asio::ssl::error::stream_truncated
                || errorCode == ::boost::beast::errc::not_connected) {
                return;
            }
            m_errorCode = errorCode;
        }

        // If we get here then the connection is closed gracefully
    }
};

} // http
} // beast
} // eboost

#endif
