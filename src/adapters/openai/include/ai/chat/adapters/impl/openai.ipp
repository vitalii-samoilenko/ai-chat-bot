#ifndef AI_CHAT_ADAPTERS_OPENAI_IPP
#define AI_CHAT_ADAPTERS_OPENAI_IPP

#include <stdexcept>
#include <thread>
#include <utility>
#include <vector>

#include "boost/scope/scope_exit.hpp"
#include "boost/json.hpp"
#include "eboost/beast.hpp"
#include "re2/re2.h"

namespace ai {
namespace chat {
namespace adapters {

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

void tag_invoke(::boost::json::value_from_tag, ::boost::json::value &value, role const &role) {
    ::boost::json::string &string{ value.emplace_string() };
    switch (role) {
        case role::system: {
            string = "system";
        } break;
        case role::user: {
            string = "user";
        } break;
        case role::assistant: {
            string = "assistant";
        } break;
        default:
            throw ::std::invalid_argument{ "role is not supported" };
    }
};
role tag_invoke(::boost::json::value_to_tag<role>, ::boost::json::value const &value) {
    ::boost::json::string const &string{ value.as_string() };
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
void tag_invoke(::boost::json::value_from_tag, ::boost::json::value &value, message const &message) {
    ::boost::json::object &object{ value.emplace_object() };
    ::boost::json::object::iterator content{
        object.emplace("content", nullptr)
            .first
    };
    ::boost::json::object::iterator role{
        object.emplace("role", nullptr)
            .first
    };
    ::boost::json::value_from(message.role, role->value());
    ::boost::json::value_from(message.content, content->value());
};
message tag_invoke(::boost::json::value_to_tag<message>, ::boost::json::value const &value) {
    return message{
        ::boost::json::value_to<role>(value.at("role")),
        ::boost::json::value_to<::std::string_view>(value.at("content")),
    };
};
usage tag_invoke(::boost::json::value_to_tag<usage>, ::boost::json::value const &value) {
    return usage{
        ::boost::json::value_to<size_t>(value.at("completion_tokens")),
        ::boost::json::value_to<size_t>(value.at("prompt_tokens")),
        ::boost::json::value_to<size_t>(value.at("total_tokens")),
    };
};
finish_reason tag_invoke(::boost::json::value_to_tag<finish_reason>, ::boost::json::value const &value) {
    ::boost::json::string const &string{ value.as_string() };
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
choice tag_invoke(::boost::json::value_to_tag<choice>, ::boost::json::value const &value) {
    return choice{
        ::boost::json::value_to<finish_reason>(value.at("finish_reason")),
        ::boost::json::value_to<size_t>(value.at("index")),
        ::boost::json::value_to<message>(value.at("message")),
    };
};
completion_result tag_invoke(::boost::json::value_to_tag<completion_result>, ::boost::json::value const &value) {
    return completion_result{
        ::boost::json::value_to<::std::vector<choice>>(value.at("choices")),
        ::boost::json::value_to<usage>(value.at("usage")),
    };
};

iterator::iterator(iterator &&other)
    : _target{ ::std::move(other._target._pos) } {

};

message iterator::operator*() {
    return ::boost::json::value_to<message>(*(_target._pos));
};
iterator & iterator::operator++() {
    ++(_target._pos);
    return *this;
};
bool iterator::operator==(iterator const &rhs) const {
    return _target._pos == rhs._target._pos;
};

iterator iterator::operator+(ptrdiff_t rhs) {
    return iterator{ (_target._pos) + rhs };
};
ptrdiff_t iterator::operator-(iterator rhs) const {
    return _target._pos - rhs._target._pos;
};

template<typename... Args>
iterator::iterator(Args &&...args)
    : _target{ ::std::forward<Args>(args)... } {

};

openai::openai(::std::string_view address, ::std::chrono::milliseconds timeout,
    ::std::chrono::milliseconds delay, size_t limit)
    : _context{} {
    ::RE2 uri{ R"(https:\/\/(?<host>[^:\/]+)(?::(?<port>\d+))?(?<path>\/.*))" };
    if (!::RE2::FullMatch(address, uri,
            &_context._host, &_context._port, &_context._t_completions)) {
        throw ::std::invalid_argument{ "invalid uri" };
    }
    if (_context._port.empty()) {
        _context._port = "443";
    }
    _context._timeout = timeout;
    _context._delay = delay;
    _context._limit = limit;
    _context.on_init();
};

iterator openai::begin() {
    ::boost::json::value &messages{ _context._completion.at("messages") };
    ::boost::json::array &array{ messages.as_array() };
    return iterator{ const_cast<::boost::json::array::iterator>(array.begin()) };
};
iterator openai::end() {
    ::boost::json::value const &messages{ _context._completion.at("messages") };
    ::boost::json::array const &array{ messages.as_array() };
    return iterator{ const_cast<::boost::json::array::iterator>(array.end()) };
};

void openai::push_back(message const &value) {
    ::opentelemetry::nostd::shared_ptr<::opentelemetry::trace::Span> span{
        _context._tracer->StartSpan("push_back")
    };
    ::boost::json::value &messages{ _context._completion.at("messages") };
    ::boost::json::array &array{ messages.as_array() };
    ::boost::json::value &value{ array.emplace_back(nullptr) };
    ::boost::json::value_from(value, value);
};
void openai::pop_back() {
    ::opentelemetry::nostd::shared_ptr<::opentelemetry::trace::Span> span{
        _context._tracer->StartSpan("pop_back")
    };
    ::boost::json::value &messages{ _context._completion.at("messages") };
    ::boost::json::array &array{ messages.as_array() };
    array.pop_back();
};

iterator openai::complete(::std::string_view model, ::std::string_view key) {
    ::opentelemetry::nostd::shared_ptr<::opentelemetry::trace::Span> span{
        _context._tracer->StartSpan("complete")
    };
    _context._completion.at("model") = model;
    _context._h_bearer.replace(::std::size("Bearer "), key.size(), key);
    ::boost::beast::http::request<::eboost::beast::http::json_body> request{
        ::boost::beast::http::verb::post, _context._t_completions, 11,
        _context._completion
    };
    request.prepare_payload();
    request.set(::boost::beast::http::field::content_type, "application/json");
    request.set(::boost::beast::http::field::authorization, _context._h_bearer);
    ::boost::beast::http::response<::eboost::beast::http::json_body> response{};
    auto on_exit = ::boost::scope::make_scope_exit([this]()->void {
        _context._io_context.restart();
        _context._stream_reset();
    });
    _context.on_send(request, response,
        span);
    _context._io_context.run();
    completion_result result{ ::boost::json::value_to<completion_result>(response.body()) };
    _context._m_context->Record(static_cast<int64_t>(result.usage.completion_tokens), {
        {"type", "completion"}
    });
    _context._m_context->Record(static_cast<int64_t>(result.usage.prompt_tokens), {
        {"type", "prompt"}
    });
    iterator pos{ end() };
    if (!(_context._limit < result.usage.total_tokens)) {
        push_back(result.choices[0].message);
    }
    return pos;
};
iterator openai::erase(iterator first, iterator last) {
    ::opentelemetry::nostd::shared_ptr<::opentelemetry::trace::Span> span{
        _context._tracer->StartSpan("erase")
    };
    ::boost::json::value &messages{ _context._completion.at("messages") };
    ::boost::json::array &array{ messages.as_array() };
    return iterator{ array.erase(first._target._pos, last._target._pos) };
};

} // adapters
} // chat
} // ai

#endif
