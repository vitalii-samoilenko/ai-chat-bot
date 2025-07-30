#ifndef AI_CHAT_CLIENTS_DETAIL_AUTH_CONNECTION_HPP
#define AI_CHAT_CLIENTS_DETAIL_AUTH_CONNECTION_HPP

#include <string>

#include "boost/asio/ssl.hpp"
#include "boost/asio.hpp"
#include "boost/beast.hpp"
#include "boost/json.hpp"
#include "eboost/beast.hpp"

#include "ai/chat/clients/auth.hpp"
#include "ai/chat/telemetry.hpp"

namespace ai {
namespace chat {
namespace clients {
namespace detail {

class auth_connection {
private:
    static size_t constexpr BUFFER_TOTAL_SIZE{ 8192 };
    static size_t constexpr BUFFER_JSON_SERIALIZER_SIZE{ BUFFER_TOTAL_SIZE };
    static size_t constexpr BUFFER_RESPONSE_PARSER_SIZE{ BUFFER_TOTAL_SIZE / 2 };
    static size_t constexpr BUFFER_READ_SIZE{ BUFFER_TOTAL_SIZE / 4 };
    static size_t constexpr BUFFER_JSON_PARSER_SIZE{ BUFFER_TOTAL_SIZE / 4 };
    static size_t constexpr BUFFER_JSON_SERIALIZER_OFFSET{ 0 };
    static size_t constexpr BUFFER_RESPONSE_PARSER_OFFSET{ 0 };
    static size_t constexpr BUFFER_READ_OFFSET{ BUFFER_RESPONSE_PARSER_OFFSET + BUFFER_RESPONSE_PARSER_SIZE };
    static size_t constexpr BUFFER_JSON_PARSER_OFFSET{ BUFFER_READ_OFFSET + BUFFER_READ_SIZE };

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
    void on_send(
        DECLARE_ONLY_SPAN(root));
    void on_write_chunk(
        DECLARE_ONLY_SPAN(span));
    void on_read_chunk(
        DECLARE_ONLY_SPAN(span));
    void on_reset();

    char _buffer[BUFFER_TOTAL_SIZE];
    ::boost::asio::io_context _io_context;
    ::boost::asio::ip::tcp::resolver _dns_resolver;
    ::boost::asio::ssl::context _ssl_context;
    ::boost::asio::ssl::stream<::eboost::beast::metered_tcp_stream<auth_connection>> _ssl_stream;
    ::boost::beast::http::request<::boost::beast::http::buffer_body> _request;
    ::boost::beast::http::response<::boost::beast::http::buffer_body> _response;
    ::boost::beast::http::request_serializer<::boost::beast::http::buffer_body> _request_serializer;
    ::boost::beast::http::response_parser<::boost::beast::http::buffer_body> _response_parser;
    ::boost::beast::flat_static_buffer_base _read_buffer;
    ::std::string _request_body;
    ::boost::json::value _response_body;
    ::boost::json::stream_parser _json_parser;
    ::std::string _host;
    ::std::string _port;
    ::std::string _path;
    ::std::string _t_validate;
    ::std::string _t_token;
    ::std::string _t_device;
    ::std::string _h_oauth;
    ::std::chrono::milliseconds _timeout;

    DELCARE_TRACER()
    DECLARE_METER()
    DECLARE_COUNTER(_m_network)
};

} // detail
} // clients
} // chat
} // ai

#include "impl/auth_connection.ipp"

#endif
