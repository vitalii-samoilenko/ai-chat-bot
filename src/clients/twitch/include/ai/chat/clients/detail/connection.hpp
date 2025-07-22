#ifndef AI_CHAT_CLIENTS_DETAIL_CONNECTION_HPP
#define AI_CHAT_CLIENTS_DETAIL_CONNECTION_HPP

#include <queue>
#include <string>

#include "boost/asio/ssl.hpp"
#include "boost/asio.hpp"
#include "boost/beast/ssl.hpp"
#include "boost/beast.hpp"
#include "boost/json.hpp"
#include "eboost/beast.hpp"
#include "re2/re2.h"

#include "ai/chat/telemetry.hpp"

namespace ai {
namespace chat {
namespace clients {
namespace detail {

template<typename Handler>
class connection {
private:
    friend ::eboost::beast::metered_rate_policy<connection>;
    friend twitch<Handler>;

    connection() = delete;
    connection(connection const &other) = delete;
    connection(connection &&other) = delete;

    ~connection() = default;

    connection & operator=(connection const &other) = delete;
    connection & operator=(connection &&other) = delete;

    connection(size_t dop, twitch<Handler> &handler);

    void bytes_rx(size_t n);
    void bytes_tx(size_t n);

    void on_init();
    void on_connect(
        DECLARE_ONLY_SPAN(root));
    void on_read();
    void on_push(::std::string &&request
        DECLARE_SPAN(root));
    void on_write(
        DECLARE_ONLY_SPAN(root));
    void on_disconnect(
        DECLARE_ONLY_SPAN(root));
    void on_send(
        DECLARE_ONLY_SPAN(root));
    void on_join(
        DECLARE_ONLY_SPAN(root));
    void on_leave(
        DECLARE_ONLY_SPAN(root));

    ::boost::asio::io_context _io_context;
    ::boost::asio::thread_pool _io_run_context;
    ::boost::asio::thread_pool _h_context;
    ::boost::asio::ip::tcp::resolver _resolver;
    ::boost::asio::ssl::context _ssl_context;
    ::boost::beast::websocket::stream<::boost::asio::ssl::stream<::eboost::beast::metered_tcp_stream<connection>>> _stream;
    ::boost::asio::signal_set _signals;
    twitch<Handler>& _handler;
    ::std::string _host;
    ::std::string _port;
    ::std::string _path;
    ::std::chrono::milliseconds _timeout;
    ::std::chrono::milliseconds _delay;
    ::std::string _authority;
    ::std::string _username;
    ::std::string _access_token;
    ::std::string _channel;
    ::std::string _message_content;
    ::std::string _message_channel;
    ::std::queue<::std::string> _q_write;
    ::boost::beast::flat_buffer _buffer;
    ::std::chrono::nanoseconds _next;
    ::RE2 _re_notice;
    ::RE2 _re_line;
    ::RE2 _re_command_wargs;
    ::RE2 _re_command;
    ::RE2 _re_message;
    ::RE2 _re_ping;
    ::RE2 _re_reconnect;

    DECLARE_LOGGER()
    DELCARE_TRACER()
    DECLARE_METER()
    DECLARE_COUNTER(_m_network)
};

} // detail
} // clients
} // chat
} // ai

#include "impl/connection.ipp"

#endif
