#ifndef AI_CHAT_ADAPTERS_OPENAI_IPP
#define AI_CHAT_ADAPTERS_OPENAI_IPP

#include <limits>
#include <stdexcept>
#include <thread>
#include <vector>

#include "boost/asio/buffer.hpp"
#include "boost/asio/ssl.hpp"
#include "boost/beast.hpp"
#include "eboost/beast/ensure_success.hpp"
#include "eboost/beast/http/json_body.hpp"
#include "eboost/beast/metered_rate_policy.hpp"
#include "opentelemetry/metrics/provider.h"
#include "opentelemetry/trace/provider.h"
#include "re2/re2.h"

#include "ai/chat/adapters/openai.hpp"

namespace ai {
namespace chat {
namespace adapters {

class openai::connection { 
public:
    // connection() = delete;
    connection(const connection&) = delete;
    connection(connection&&) = delete;

    ~connection() = default;

    connection& operator=(const connection&) = delete;
    connection& operator=(connection&&) = delete;

private:
    friend ::eboost::beast::metered_rate_policy<connection>;
    friend openai;

    connection()
        : _io_context{}
        , _resolver{ _io_context }
        , _ssl_context{ ::boost::asio::ssl::context::tlsv12_client }
        , _stream{ ::eboost::beast::metered_tcp_stream<connection>{ ::eboost::beast::metered_rate_policy<connection>{ *this }, _io_context }, _ssl_context }
        , _tracer{
            ::opentelemetry::trace::Provider::GetTracerProvider()
                ->GetTracer("ai_chat_adapters_openai")
        }
        , _meter{
            ::opentelemetry::metrics::Provider::GetMeterProvider()
                ->GetMeter("ai_chat_adapters_openai")
        }
        , _m_context{ _meter->CreateInt64Gauge("ai_chat_adapters_openai_context_total") }
        , _m_network{ _meter->CreateUInt64Counter("ai_chat_adapters_openai_network") }
        , _host{}
        , _port{}
        , _path{}
        , _timeout{}
        , _delay{}
        , _buffer{}
        , _next{}
        , _completion{
            {"model", ::boost::json::string{}},
            {"messages", ::boost::json::array{}}
        } {

    };

    ::boost::asio::io_context _io_context;
    ::boost::asio::ip::tcp::resolver _resolver;
    ::boost::asio::ssl::context _ssl_context;
    ::boost::asio::ssl::stream<::eboost::beast::metered_tcp_stream<connection>> _stream;
    ::opentelemetry::nostd::shared_ptr<::opentelemetry::trace::Tracer> _tracer;
    ::opentelemetry::nostd::shared_ptr<::opentelemetry::metrics::Meter> _meter;
    ::opentelemetry::nostd::unique_ptr<::opentelemetry::metrics::Gauge<int64_t>> _m_context;
    ::opentelemetry::nostd::unique_ptr<::opentelemetry::metrics::Counter<uint64_t>> _m_network;
    ::std::string _host;
    ::std::string _port;
    ::std::string _path;
    ::std::chrono::milliseconds _timeout;
    ::std::chrono::milliseconds _delay;
    ::boost::beast::flat_buffer _buffer;
    ::std::chrono::nanoseconds _next;
    ::boost::json::value _completion;

    void bytes_rx(size_t n) {
        _m_network->Add(n,
        {
            {"type", "rx"}
        });
    };
    void bytes_tx(size_t n) {
        _m_network->Add(n,
        {
            {"type", "tx"}
        });
    };

    void on_init() {
        _ssl_context.set_verify_mode(::boost::asio::ssl::verify_none);
    };
    template<typename Request, typename Response>
    void on_send(Request& request, Response& response,
        ::opentelemetry::nostd::shared_ptr<::opentelemetry::trace::Span> root) {
        ::opentelemetry::nostd::shared_ptr<::opentelemetry::trace::Span> span{
            _tracer->StartSpan("on_send", ::opentelemetry::trace::StartSpanOptions
            {
                {}, {},
                root->GetContext()
            })
        };
        ::std::string target{ _path };
        request.target(target.append(request.target()));
        request.set(::boost::beast::http::field::host, _host);
        request.set(::boost::beast::http::field::user_agent, BOOST_BEAST_VERSION_STRING);
        if (!::SSL_set_tlsext_host_name(_stream.native_handle(), _host.c_str())) {
            throw ::boost::beast::system_error{
                static_cast<int>(::ERR_get_error()),
                ::boost::asio::error::get_ssl_category()
            };
        }
        _stream.set_verify_callback(::boost::asio::ssl::host_name_verification{ _host });
        auto now = ::std::chrono::steady_clock::now();
        if (now.time_since_epoch() < _next) {
            ::std::this_thread::sleep_for(_next - now.time_since_epoch());
        }
        ::boost::beast::get_lowest_layer(_stream).expires_after(_timeout);
        ::opentelemetry::nostd::shared_ptr<::opentelemetry::trace::Span> operation{
            _tracer->StartSpan("on_dns_resolve", ::opentelemetry::trace::StartSpanOptions
            {
                {}, {},
                span->GetContext()
            })
        };
        _resolver.async_resolve(_host, _port, [this, &request, &response, span, operation](::boost::beast::error_code error_code, ::boost::asio::ip::tcp::resolver::results_type results) mutable ->void {
        ::eboost::beast::ensure_success(error_code);
        operation = _tracer->StartSpan("on_tcp_connect", ::opentelemetry::trace::StartSpanOptions
        {
            {}, {},
            span->GetContext()
        });
        ::boost::beast::get_lowest_layer(_stream).async_connect(results, [this, &request, &response, span, operation](::boost::beast::error_code error_code, ::boost::asio::ip::tcp::resolver::results_type::endpoint_type) mutable ->void {
        ::eboost::beast::ensure_success(error_code);
        operation = _tracer->StartSpan("on_ssl_handshake", ::opentelemetry::trace::StartSpanOptions
        {
            {}, {},
            span->GetContext()
        });
        _stream.async_handshake(::boost::asio::ssl::stream_base::client, [this, &request, &response, span, operation](::boost::beast::error_code error_code) mutable ->void {
        ::eboost::beast::ensure_success(error_code);
        operation = _tracer->StartSpan("on_write", ::opentelemetry::trace::StartSpanOptions
        {
            {}, {},
            span->GetContext()
        });
        ::boost::beast::http::async_write(_stream, request, [this, &response, span, operation](::boost::beast::error_code error_code, size_t bytes_transferred) mutable ->void {
        ::eboost::beast::ensure_success(error_code);
        ::boost::ignore_unused(bytes_transferred);
        operation = _tracer->StartSpan("on_read", ::opentelemetry::trace::StartSpanOptions
        {
            {}, {},
            span->GetContext()
        });
        ::boost::beast::http::async_read(_stream, _buffer, response, [this, span, operation](::boost::beast::error_code error_code, size_t bytes_transferred) mutable ->void {
        ::eboost::beast::ensure_success(error_code);
        ::boost::ignore_unused(bytes_transferred);
        operation = nullptr;
        auto now = ::std::chrono::steady_clock::now();
        _next = now.time_since_epoch() + _delay;
        operation = _tracer->StartSpan("on_shutdown", ::opentelemetry::trace::StartSpanOptions
        {
            {}, {},
            span->GetContext()
        });
        _stream.async_shutdown([this, span, operation](::boost::beast::error_code error_code) mutable ->void {
        operation = nullptr;
        _stream.~stream();
        new(&_stream) ::boost::asio::ssl::stream<::eboost::beast::metered_tcp_stream<connection>>{
            ::eboost::beast::metered_tcp_stream<connection>{ ::eboost::beast::metered_rate_policy<connection>{ *this }, _io_context },
            _ssl_context
        };
        if (error_code == ::boost::asio::ssl::error::stream_truncated) {
            return;
        }
        ::eboost::beast::ensure_success(error_code);

        }); // shutdown
        }); // read
        }); // write
        }); // SSL handshake
        }); // TCP connect
        }); // resolve
    };
};

openai::openai(const ::std::string& address, ::std::chrono::milliseconds timeout,
    ::std::chrono::milliseconds delay)
    : _context{ new connection{} } {
    ::RE2 uri{ R"(https:\/\/(?<host>[^:\/]+)(?::(?<port>\d+))?(?<path>\/.*))" };
    if (!::RE2::FullMatch(address, uri,
            &_context->_host, &_context->_port, &_context->_path)) {
        throw ::std::invalid_argument{ "invalid uri" };
    }
    if (_context->_port.empty()) {
        _context->_port = "443";
    }
    _context->_timeout = timeout;
    _context->_delay = delay;
    _context->on_init();
};

enum class finish_reason {
    stop,
    length,
    content_filter,
    tool_calls
};
struct choice {
    ::ai::chat::adapters::finish_reason finis_reason;
    size_t index;
    ::ai::chat::adapters::message message;
};
struct usage {
    size_t completion_tokens;
    size_t prompt_tokens;
    size_t total_tokens;
};
struct completion_result {
    ::std::vector<choice> choices;
    ::ai::chat::adapters::usage usage;
};

::std::string to_string(const role& role) {
    switch (role) {
        case role::system:
            return "system";
        case role::user:
            return "user";
        case role::assistant:
            return "assistant";
    }
    throw ::std::invalid_argument{ "role is not supported" };
};
role tag_invoke(::boost::json::value_to_tag<role>, const ::boost::json::value& value) {
    const ::boost::json::string& string{ value.as_string() };
    if (string == "system") {
        return role::system;
    }
    if (string == "user") {
        return role::user;
    }
    if (string == "assistant") {
        return role::assistant;
    }
    throw ::std::invalid_argument{ "role is not supported" };
};

void tag_invoke(::boost::json::value_from_tag, ::boost::json::value& value, const message& message) {
    value = {
        { "role" , to_string(message.role) },
        { "content", message.content },
    };
};
message tag_invoke(::boost::json::value_to_tag<message>, const ::boost::json::value& value) {
    return {
        ::boost::json::value_to<role>(value.at("role")),
        ::boost::json::value_to<::std::string>(value.at("content")),
    };
};

usage tag_invoke(::boost::json::value_to_tag<usage>, const ::boost::json::value& value) {
    return {
        ::boost::json::value_to<size_t>(value.at("completion_tokens")),
        ::boost::json::value_to<size_t>(value.at("prompt_tokens")),
        ::boost::json::value_to<size_t>(value.at("total_tokens")),
    };
};

finish_reason tag_invoke(::boost::json::value_to_tag<finish_reason>, const ::boost::json::value& value) {
    const ::boost::json::string& string{ value.as_string() };
    if (string == "stop") {
        return finish_reason::stop;
    }
    if (string == "length") {
        return finish_reason::length;
    }
    if (string == "content_filter") {
        return finish_reason::content_filter;
    }
    if (string == "tool_calls") {
        return finish_reason::tool_calls;
    }
    throw ::std::invalid_argument{ "finish reason is not supported" };
};

choice tag_invoke(::boost::json::value_to_tag<choice>, const ::boost::json::value& value) {
    return {
        ::boost::json::value_to<finish_reason>(value.at("finish_reason")),
        ::boost::json::value_to<size_t>(value.at("index")),
        ::boost::json::value_to<message>(value.at("message")),
    };
};

completion_result tag_invoke(::boost::json::value_to_tag<completion_result>, const ::boost::json::value& value) {
    return {
        ::boost::json::value_to<::std::vector<choice>>(value.at("choices")),
        ::boost::json::value_to<usage>(value.at("usage")),
    };
};

openai::iterator openai::insert(const message& message) {
    ::opentelemetry::nostd::shared_ptr<::opentelemetry::trace::Span> span{
        _context->_tracer->StartSpan("insert")
    };
    ::boost::json::array& messages{ _context->_completion.at("messages").as_array() };
    messages.push_back(::boost::json::value_from(message));
    return messages.size() - 1;
};
message openai::complete(const ::std::string& model, const ::std::string& key) {
    ::opentelemetry::nostd::shared_ptr<::opentelemetry::trace::Span> span{
        _context->_tracer->StartSpan("complete")
    };
    _context->_completion.at("model") = model;
    ::boost::beast::http::request<::eboost::beast::http::json_body> request{
        ::boost::beast::http::verb::post, "chat/completions", 11,
        _context->_completion
    };
    request.prepare_payload();
    request.set(::boost::beast::http::field::content_type, "application/json");
    request.set(::boost::beast::http::field::authorization, "Bearer " + key);
    ::boost::beast::http::response<::eboost::beast::http::json_body> response{};
    _context->on_send(request, response,
        span);
    _context->_io_context.run();
    _context->_io_context.restart();
    completion_result result{ ::boost::json::value_to<completion_result>(response.body()) };
    _context->_m_context->Record(static_cast<int64_t>(result.usage.completion_tokens),
    {
        {"type", "completion"}
    });
    _context->_m_context->Record(static_cast<int64_t>(result.usage.prompt_tokens),
    {
        {"type", "prompt"}
    });
    return result.choices[0].message;
};

} // adapters
} // chat
} // ai

#endif
