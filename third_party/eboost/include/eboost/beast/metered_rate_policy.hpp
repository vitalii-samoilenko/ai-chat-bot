#ifndef EBOOST_BEAST_METERED_RATE_POLICY_HPP
#define EBOOST_BEAST_METERED_RATE_POLICY_HPP

#include <boost/asio/any_io_executor.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/beast/core/basic_stream.hpp>

namespace eboost {
namespace beast {

template<typename Connection>
class metered_rate_policy {
public:
    metered_rate_policy() = delete;
    metered_rate_policy(const metered_rate_policy&) = delete;
    metered_rate_policy(metered_rate_policy&&) = default;

    ~metered_rate_policy() = default;

    metered_rate_policy& operator=(const metered_rate_policy&) = delete;
    metered_rate_policy& operator=(metered_rate_policy&&) = delete;

private:
    friend ::boost::beast::rate_policy_access;
    friend Connection;

    explicit metered_rate_policy(Connection& connection)
        : _connection{ connection } {

    };

    Connection& _connection;

    size_t available_read_bytes() const {
        return ::std::numeric_limits<size_t>::max();
    };
    size_t available_write_bytes() const {
        return ::std::numeric_limits<size_t>::max();
    };
    void transfer_read_bytes(size_t n) {
        _connection.bytes_rx(n);
    };
    void transfer_write_bytes(size_t n) {
        _connection.bytes_tx(n);
    };
    void on_timer() const {

    };
};

template<typename Connection>
using metered_tcp_stream = ::boost::beast::basic_stream<
    ::boost::asio::ip::tcp,
    ::boost::asio::any_io_executor,
    metered_rate_policy<Connection>>;

} // beast
} // boost

#endif
