#ifndef AI_CHAT_ADAPTERS_DETAIL_CONNECTION_HPP
#define AI_CHAT_ADAPTERS_DETAIL_CONNECTION_HPP

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
namespace adapters {

class iterator;
class openai;

namespace detail {

class connection {
private:
    friend ::eboost::beast::metered_rate_policy<connection>;
    friend iterator;
    friend openai;

    connection();
    connection(connection const &other) = delete;
    connection(connection &&other) = delete;

    ~connection() = default;

    connection & operator=(connection const &other) = delete;
    connection & operator=(connection &&other) = delete;

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
    ::boost::asio::ssl::stream<::eboost::beast::metered_tcp_stream<connection>> _stream;
    ::boost::beast::flat_buffer _buffer;
    ::boost::json::value _completion;
    ::opentelemetry::nostd::shared_ptr<::opentelemetry::trace::Tracer> _tracer;
    ::opentelemetry::nostd::shared_ptr<::opentelemetry::metrics::Meter> _meter;
    ::opentelemetry::nostd::unique_ptr<::opentelemetry::metrics::Gauge<int64_t>> _m_context;
    ::opentelemetry::nostd::unique_ptr<::opentelemetry::metrics::Counter<uint64_t>> _m_network;
    ::std::string _host;
    ::std::string _port;
    ::std::string _t_completions;
    ::std::string _h_bearer;
    ::std::chrono::milliseconds _timeout;
    ::std::chrono::milliseconds _delay;
    ::std::chrono::nanoseconds _next;
    size_t _limit;
};

} // detail
} // adapters
} // chat
} // ai

#include "impl/connection.ipp"

#endif
