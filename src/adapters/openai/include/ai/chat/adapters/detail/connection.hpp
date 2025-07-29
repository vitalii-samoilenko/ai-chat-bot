#ifndef AI_CHAT_ADAPTERS_DETAIL_CONNECTION_HPP
#define AI_CHAT_ADAPTERS_DETAIL_CONNECTION_HPP

#include <string>

#include "boost/asio/ssl.hpp"
#include "boost/asio.hpp"
#include "boost/beast.hpp"
#include "boost/json.hpp"
#include "eboost/beast.hpp"

#include "ai/chat/adapters/openai.hpp"
#include "ai/chat/telemetry.hpp"

namespace ai {
namespace chat {
namespace adapters {
namespace detail {

class connection {
private:
    static constexpr size_t BUFFER_SIZE{ 8192 };
    static constexpr size_t RESPONSE_SIZE{ BUFFER_SIZE / 2 };
    static constexpr size_t READ_SIZE{ BUFFER_SIZE / 4 };
    static constexpr size_t JSON_SIZE{ BUFFER_SIZE / 4 };

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
    void on_send(
        DECLARE_ONLY_SPAN(root));
    void on_write_chunk(
        DECLARE_ONLY_SPAN(span));
    void on_read_chunk(
        DECLARE_ONLY_SPAN(span));
    void on_reset();

    char _buffer[BUFFER_SIZE];
    ::boost::asio::io_context _io_context;
    ::boost::asio::ip::tcp::resolver _dns_resolver;
    ::boost::asio::ssl::context _ssl_context;
    ::boost::asio::ssl::stream<::eboost::beast::metered_tcp_stream<connection>> _ssl_stream;
    ::boost::beast::http::request<::boost::beast::http::buffer_body> _request;
    ::boost::beast::http::response<::boost::beast::http::buffer_body> _response;
    ::boost::beast::http::request_serializer<::boost::beast::http::buffer_body> _request_serializer;
    ::boost::beast::http::response_parser<::boost::beast::http::buffer_body> _response_parser;
    ::boost::beast::flat_static_buffer_base _read_buffer;
    ::boost::json::value _request_body;
    ::boost::json::value _response_body;
    ::boost::json::serializer _json_serializer;
    ::boost::json::stream_parser _json_parser;
    ::std::string _host;
    ::std::string _port;
    ::std::string _t_completions;
    ::std::string _h_bearer;
    ::std::chrono::milliseconds _timeout;
    ::std::chrono::milliseconds _delay;
    ::std::chrono::nanoseconds _next;
    size_t _total_limit;

    DECLARE_LOGGER()
    DELCARE_TRACER()
    DECLARE_METER()
    DECLARE_GAUGE(_m_context)
    DECLARE_COUNTER(_m_network)
};

} // detail
} // adapters
} // chat
} // ai

#include "impl/connection.ipp"

#endif
