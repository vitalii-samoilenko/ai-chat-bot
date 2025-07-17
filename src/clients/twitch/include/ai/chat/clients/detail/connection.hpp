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
#include "opentelemetry/logs/provider.h"
#include "opentelemetry/metrics/provider.h"
#include "opentelemetry/trace/provider.h"
#include "re2/re2.h"

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
        ::opentelemetry::nostd::shared_ptr<::opentelemetry::trace::Span> root);
    void on_read();
    void on_push(::std::string &&request,
        ::opentelemetry::nostd::shared_ptr<::opentelemetry::trace::Span> root);
    void on_write(
        ::opentelemetry::nostd::shared_ptr<::opentelemetry::trace::Span> root);
    void on_disconnect(
        ::opentelemetry::nostd::shared_ptr<::opentelemetry::trace::Span> root);
    void on_send(
        ::opentelemetry::nostd::shared_ptr<::opentelemetry::trace::Span> root);
    void on_join(
        ::opentelemetry::nostd::shared_ptr<::opentelemetry::trace::Span> root);
    void on_leave(
        ::opentelemetry::nostd::shared_ptr<::opentelemetry::trace::Span> root);

    ::boost::asio::io_context _io_context;
    ::boost::asio::thread_pool _io_run_context;
    ::boost::asio::thread_pool _h_context;
    ::boost::asio::ip::tcp::resolver _resolver;
    ::boost::asio::ssl::context _ssl_context;
    ::boost::beast::websocket::stream<::boost::asio::ssl::stream<::eboost::beast::metered_tcp_stream<connection>>> _stream;
    ::boost::asio::signal_set _signals;
    twitch<Handler>& _handler;
    ::opentelemetry::nostd::shared_ptr<::opentelemetry::logs::Logger> _logger;
    ::opentelemetry::nostd::shared_ptr<::opentelemetry::trace::Tracer> _tracer;
    ::opentelemetry::nostd::shared_ptr<::opentelemetry::metrics::Meter> _meter;
    ::opentelemetry::nostd::unique_ptr<::opentelemetry::metrics::Counter<uint64_t>> _m_network;
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
};

} // detail
} // clients
} // chat
} // ai

#include "impl/connection.ipp"

#endif
