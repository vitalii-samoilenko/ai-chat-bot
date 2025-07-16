#ifndef AI_CHAT_CLIENTS_DETAIL_AUTH_CONNECTION_HPP
#define AI_CHAT_CLIENTS_DETAIL_AUTH_CONNECTION_HPP

#include <string>

#include "boost/asio/ssl.hpp"
#include "boost/asio.hpp"
#include "boost/beast.hpp"
#include "boost/json.hpp"
#include "eboost/beast.hpp"
#include "opentelemetry/metrics/provider.h"
#include "opentelemetry/trace/provider.h"

namespace ai {
namespace chat {
namespace clients {
namespace detail {

class auth_connection {
private:
    friend ::eboost::beast::metered_rate_policy<auth_connection>;
    friend auth;

    auth_connection();
    auth_connection(auth_connection const &other) = delete;
    auth_connection(auth_connection &&other) = delete;

    ~auth_connection() = default;

    auth_connection & operator=(auth_connection const &other) = delete;
    auth_connection & operator=(auth_connection &&other) = delete;

    void bytes_rx(size_t n);
    void bytes_tx(size_t n);

    void on_init();
    template<typename Request, typename Response>
    void on_send(Request &request, Response &response,
        ::opentelemetry::nostd::shared_ptr<::opentelemetry::trace::Span> root);
    void _stream_reset();

    ::boost::asio::io_context _io_context;
    ::boost::asio::ip::tcp::resolver _resolver;
    ::boost::asio::ssl::context _ssl_context;
    ::boost::asio::ssl::stream<::eboost::beast::metered_tcp_stream<auth_connection>> _stream;
    ::boost::beast::flat_buffer _buffer;
    ::boost::json::value _context;
    ::opentelemetry::nostd::shared_ptr<::opentelemetry::trace::Tracer> _tracer;
    ::opentelemetry::nostd::shared_ptr<::opentelemetry::metrics::Meter> _meter;
    ::opentelemetry::nostd::unique_ptr<::opentelemetry::metrics::Counter<uint64_t>> _m_network;
    ::std::string _host;
    ::std::string _port;
    ::std::string _path;
    ::std::string _t_validate;
    ::std::string _t_token;
    ::std::string _t_device;
    ::std::string _h_oauth;
    ::std::chrono::milliseconds _timeout;
};

} // detail
} // clients
} // chat
} // ai

#include "impl/auth_connection.ipp"

#endif
