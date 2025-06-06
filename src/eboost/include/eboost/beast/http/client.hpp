#ifndef EBOOST_BEAST_HTTP_CLIENT_HPP
#define EBOOST_BEAST_HTTP_CLIENT_HPP

#include <string>

#include <boost/asio/ssl.hpp>
#include <boost/beast.hpp>

namespace eboost {
namespace beast {
namespace http {

class client {
    client() = delete;
    client(const client&) = delete;
    client(client&&) = delete;

    ~client() = delete;

    client& operator=(const client&) = delete;
    client& operator=(client&&) = delete;

public:
    template<typename TRequestBody, typename TResponseBody>
    static ::boost::beast::http::response<TResponseBody> send(const ::std::string& host, const ::std::string& port, const ::boost::beast::http::request<TRequestBody>& request) {
        ::boost::asio::io_context ioc{};
        ::boost::asio::ip::tcp::resolver resolver{ ioc };
        ::boost::beast::tcp_stream stream{ ioc };

        auto const results = resolver.resolve(host, port);
        stream.connect(results);

        ::boost::beast::http::write(stream, request);

        ::boost::beast::flat_buffer buffer{};
        ::boost::beast::http::response<TResponseBody> response{};
        ::boost::beast::http::read(stream, buffer, response);

        ::boost::beast::error_code ec{};
        stream.socket().shutdown(::boost::asio::ip::tcp::socket::shutdown_both, ec);
        if(ec && !(ec == ::boost::beast::errc::not_connected))
            throw ::boost::beast::system_error{ ec} ;
        
        return response;
    }

    template<typename TRequestBody, typename TResponseBody>
    static ::boost::beast::http::response<TResponseBody> send_secure(const ::std::string& host, const ::std::string& port, const ::boost::beast::http::request<TRequestBody>& request) {
        ::boost::asio::io_context ioc{};
        ::boost::asio::ssl::context ctx{ ::boost::asio::ssl::context::tlsv12_client };
        ctx.set_verify_mode(::boost::asio::ssl::verify_none);
        ::boost::asio::ip::tcp::resolver resolver{ ioc };
        ::boost::asio::ssl::stream<::boost::beast::tcp_stream> stream{ ioc, ctx };
        if (!SSL_set_tlsext_host_name(stream.native_handle(), host.c_str())) {
            throw ::boost::beast::system_error{
                static_cast<int>(::ERR_get_error()),
                ::boost::asio::error::get_ssl_category()
            };
        }
        stream.set_verify_callback(::boost::asio::ssl::host_name_verification(host));

        auto const results = resolver.resolve(host, port);
        ::boost::beast::get_lowest_layer(stream).connect(results);
        stream.handshake(::boost::asio::ssl::stream_base::client);

        ::boost::beast::http::write(stream, request);

        ::boost::beast::flat_buffer buffer{};
        ::boost::beast::http::response<TResponseBody> response{};
        ::boost::beast::http::read(stream, buffer, response);

        ::boost::beast::error_code ec{};
        stream.shutdown(ec);
        if (!(ec == ::boost::asio::ssl::error::stream_truncated))
            throw ::boost::beast::system_error{ ec };

        return response;
    }
};

} // http
} // beast
} // eboost

#endif
