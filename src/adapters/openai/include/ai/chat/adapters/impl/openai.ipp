#ifndef AI_CHAT_ADAPTERS_OPENAI_IPP
#define AI_CHAT_ADAPTERS_OPENAI_IPP

#include <stdexcept>
#include <vector>

#include "boost/asio/buffer.hpp"
#include "boost/asio/ssl.hpp"
#include "boost/beast.hpp"
#include "boost/url.hpp"

#include "eboost/beast/ensure_success.hpp"
#include "eboost/beast/http/json_body.hpp"

#include "ai/chat/adapters/openai.hpp"

namespace ai {
namespace chat {
namespace adapters {

class openai::connection {
    friend openai;

public:
    // connection() = delete;
    connection(const connection&) = delete;
    connection(connection&&) = delete;

    ~connection() = default;

    connection& operator=(const connection&) = delete;
    connection& operator=(connection&&) = delete;

private:
    connection()
        : _context{}
        , _resolver{ _context }
        , _ssl_context{ ::boost::asio::ssl::context::tlsv12_client }
        , _stream{ _context, _ssl_context }
        , _host{}
        , _port{}
        , _path{}
        , _timeout{}
        , _buffer{}
        , _completion{
            {"model", ::boost::json::string{}},
            {"messages", ::boost::json::array{}}
        } {
        
    };

    ::boost::asio::io_context _context;
    ::boost::asio::ip::tcp::resolver _resolver;
    ::boost::asio::ssl::context _ssl_context;
    ::boost::asio::ssl::stream<::boost::beast::tcp_stream> _stream;
    ::std::string _host;
    ::std::string _port;
    ::std::string _path;
    ::std::chrono::milliseconds _timeout;
    ::boost::beast::flat_buffer _buffer;
    ::boost::json::value _completion;

    void on_init() {
        _ssl_context.set_verify_mode(::boost::asio::ssl::verify_none);
    };
    template<typename Request, typename Response>
    void on_connect(Request& request, Response& response) {
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

        ::boost::beast::get_lowest_layer(_stream).expires_after(_timeout);
        _resolver.async_resolve(_host, _port,
            [this, &request, &response](::boost::beast::error_code error_code, ::boost::asio::ip::tcp::resolver::results_type results)->void {
                ::eboost::beast::ensure_success(error_code);
                on_resolve(request, response, ::std::move(results));
            });
    };
    template<typename Request, typename Response>
    void on_resolve(Request& request, Response& response, ::boost::asio::ip::tcp::resolver::results_type results) {
        ::boost::beast::get_lowest_layer(_stream).async_connect(results,
            [this, &request, &response](::boost::beast::error_code error_code, ::boost::asio::ip::tcp::resolver::results_type::endpoint_type endpoint_type)->void {
                ::eboost::beast::ensure_success(error_code);
                on_transport_connect(request, response, ::std::move(endpoint_type));
            });
    };
    template<typename Request, typename Response>
    void on_transport_connect(Request& request, Response& response, ::boost::asio::ip::tcp::resolver::results_type::endpoint_type) {
        _stream.async_handshake(::boost::asio::ssl::stream_base::client,
            [this, &request, &response](::boost::beast::error_code error_code)->void {
                ::eboost::beast::ensure_success(error_code);
                on_ssl_handshake(request, response);
            });
    };
    template<typename Request, typename Response>
    void on_ssl_handshake(Request& request, Response& response) {
        ::boost::beast::http::async_write(_stream, request,
            [this, &response](::boost::beast::error_code error_code, size_t bytes_transferred)->void {
                ::eboost::beast::ensure_success(error_code);
                on_write(response, bytes_transferred);
            });
    };
    template<typename Response>
    void on_write(Response& response, size_t bytes_transferred) {
        ::boost::ignore_unused(bytes_transferred);
        ::boost::beast::http::async_read(_stream, _buffer, response,
            [this](::boost::beast::error_code error_code, size_t bytes_transferred)->void {
                ::eboost::beast::ensure_success(error_code);
                on_read(bytes_transferred);
            });
    };
    void on_read(size_t bytes_transferred) {
        ::boost::ignore_unused(bytes_transferred);
        _stream.async_shutdown(
            [this](::boost::beast::error_code error_code)->void {
                _stream.~stream();
                new(&_stream) ::boost::asio::ssl::stream<::boost::beast::tcp_stream>{ _context, _ssl_context };
                
                if (error_code == ::boost::asio::ssl::error::stream_truncated) {
                    return;
                }
                ::eboost::beast::ensure_success(error_code);
            });
    };
};

openai::openai(const ::std::string& address, ::std::chrono::milliseconds timeout)
    : _p_context{ new connection{} } {
    ::boost::system::result<::boost::urls::url_view> result{ ::boost::urls::parse_uri(address) };
    if (!result.has_value()) {
        throw ::std::invalid_argument{ "invalid uri" };
    }
    ::boost::urls::url_view url{ result.value() };
    if (!(url.scheme() == "https")) {
        throw ::std::invalid_argument{ "scheme is not supported" };
    }
    _p_context->_host = url.host();
    _p_context->_port = url.has_port()
        ? url.port()
        : "443";
    _p_context->_path = url.path();
    _p_context->_timeout = timeout;
    _p_context->on_init();
};

enum class finish_reason {
    stop,
    length,
    content_filter,
    tool_calls
};
struct choice {
    finish_reason finis_reason;
    size_t index;
    message message;
};
struct usage {
    size_t completion_tokens;
    size_t prompt_tokens;
    size_t total_tokens;
};
struct completion_result {
    ::std::vector<choice> choices;
    usage usage;
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
    ::boost::json::array& messages{ _p_context->_completion.at("messages").as_array() };
    messages.push_back(::boost::json::value_from(message));
    return messages.size() - 1;
};
message openai::complete(const ::std::string& model, const ::std::string& key) {
    _p_context->_completion.at("model") = model;
    ::boost::beast::http::request<::eboost::beast::http::json_body> request{
        ::boost::beast::http::verb::post, "chat/completions", 11,
        _p_context->_completion
    };
    request.prepare_payload();
    request.set(::boost::beast::http::field::content_type, "application/json");
    request.set(::boost::beast::http::field::authorization, "Bearer " + key);

    ::boost::beast::http::response<::eboost::beast::http::json_body> response{};

    _p_context->on_connect(request, response);
    _p_context->_context.run();
    _p_context->_context.restart();
    
    completion_result result{ ::boost::json::value_to<completion_result>(response.body()) };

    return result.choices[0].message;
};

} // adapters
} // chat
} // ai

#endif
