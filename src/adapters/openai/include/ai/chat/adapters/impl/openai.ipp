#ifndef AI_CHAT_ADAPTERS_OPENAI_IPP
#define AI_CHAT_ADAPTERS_OPENAI_IPP

#include <stdexcept>
#include <thread>
#include <utility>

#include "boost/scope/scope_exit.hpp"
#include "eboost/beast.hpp"
#include "re2/re2.h"

namespace ai {
namespace chat {
namespace adapters {

struct usage {
    size_t completion_tokens;
    size_t prompt_tokens;
    size_t total_tokens;
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
    ::boost::json::value_from(message.content, content->value());
    ::boost::json::object::iterator role{
        object.emplace("role", nullptr)
            .first
    };
    ::boost::json::value_from(message.role, role->value());
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

iterator::iterator(iterator const &other)
    : _target{ other._target._pos } {

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

iterator iterator::operator+(ptrdiff_t rhs) const {
    return iterator{ _target._pos + rhs };
};
ptrdiff_t iterator::operator-(iterator rhs) const {
    return _target._pos - rhs._target._pos;
};

iterator & iterator::operator=(::std::string_view rhs) {
    ::boost::json::value &content{ _target._pos->at("content") };
    content.as_string() = rhs;
    return *this;
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

message openai::front() {
    ::boost::json::value &messages{ _context._completion.at("messages") };
    ::boost::json::array &array{ messages.as_array() };
    return ::boost::json::value_to<message>(array.front());
};
message openai::back() {
    ::boost::json::value &messages{ _context._completion.at("messages") };
    ::boost::json::array &array{ messages.as_array() };
    return ::boost::json::value_to<message>(array.back());
};

iterator openai::begin() {
    ::boost::json::value &messages{ _context._completion.at("messages") };
    ::boost::json::array &array{ messages.as_array() };
    return iterator{ array.begin() };
};
iterator openai::end() {
    ::boost::json::value &messages{ _context._completion.at("messages") };
    ::boost::json::array &array{ messages.as_array() };
    return iterator{ array.end() };
};

void openai::reserve(size_t capacity) {
    ::boost::json::value &messages{ _context._completion.at("messages") };
    ::boost::json::array &array{ messages.as_array() };
    array.reserve(capacity);
};

void openai::push_back(message value) {
    ::opentelemetry::nostd::shared_ptr<::opentelemetry::trace::Span> span{
        _context._tracer->StartSpan("push_back")
    };
    _context._logger->Info(::opentelemetry::nostd::string_view{ value.content.data(), value.content.size() }, span->GetContext());
    ::boost::json::value &messages{ _context._completion.at("messages") };
    ::boost::json::array &array{ messages.as_array() };
    ::boost::json::value_from(value, array.emplace_back(nullptr));
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
    _context._h_bearer.resize(::std::size("Bearer ") - 1);
    _context._h_bearer += key;
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
    ::boost::json::value &_usage{ response.body().at("usage") };
    ::ai::chat::adapters::usage usage{ ::boost::json::value_to<::ai::chat::adapters::usage>(_usage) };
    _context._m_context->Record(static_cast<int64_t>(usage.completion_tokens), {
        {"type", "completion"}
    });
    _context._m_context->Record(static_cast<int64_t>(usage.prompt_tokens), {
        {"type", "prompt"}
    });
    if (_context._limit < usage.total_tokens) {
        return end();
    }
    ::boost::json::value &_choices{ response.body().at("choices") };
    ::boost::json::array &_array{ _choices.as_array() };
    ::boost::json::value &_choice{ _array[0] };
    ::boost::json::value &_message{ _choice.at("message") };
    push_back(::boost::json::value_to<::ai::chat::adapters::message>(_message));
    return end() + -1;
};
iterator openai::erase(iterator pos) {
    ::opentelemetry::nostd::shared_ptr<::opentelemetry::trace::Span> span{
        _context._tracer->StartSpan("erase")
    };
    ::boost::json::value &messages{ _context._completion.at("messages") };
    ::boost::json::array &array{ messages.as_array() };
    return iterator{ array.erase(pos._target._pos) };
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
