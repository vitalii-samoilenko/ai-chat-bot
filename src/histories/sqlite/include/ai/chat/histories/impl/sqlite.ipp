#ifndef AI_CHAT_HISTORIES_SQLITE_IPP
#define AI_CHAT_HISTORIES_SQLITE_IPP

#include <limits>
#include <stdexcept>
#include <tuple>
#include <utility>

#include "sqlite3.h"

namespace ai {
namespace chat {
namespace histories {

iterator::iterator(iterator &&other)
    : _target{ other._target._database } {
    _target._timestamp = other._target._timestamp;
    _target._commit = other._target._commit;
    _target._rollback = other._target._rollback;
    _target._exceptions = other._target._exceptions;
    _target._s_message = other._target._s_message;
    _target._s_message_tag = other._target._s_message_tag;
    _target._s_tag_name = other._target._s_tag_name;
    _target._s_tag_value = other._target._s_tag_value;
    _target._s_count = other._target._s_count;
    _target._tag_names = ::std::move(other._target._tag_names);
    _target._tag_values = ::std::move(other._target._tag_values);
    _target._content = ::std::move(other._target._content);
    _target._tags = ::std::move(other._target._tags);
    other._target._commit = nullptr;
    other._target._rollback = nullptr;
    other._target._s_message = nullptr;
};

iterator::~iterator() {
    if (!_target._commit) {
        return;
    }
    if (!(_target._exceptions == ::std::uncaught_exceptions())) {
        _target.on_rollback();
    }
    _target.on_commit();
};

message iterator::operator*() {
    return message{
        _target._timestamp,
        _target._content,
        _target._tags
    };
};
iterator & iterator::operator++() {
    _target.on_advance();
    return *this;
};
bool iterator::operator==(iterator const &rhs) const {
    return _target._database == rhs._target._database
        && _target._timestamp == rhs._target._timestamp;
};

iterator & iterator::operator+(ptrdiff_t rhs) {
    _target.on_advance(rhs);
    return *this;
};
iterator & iterator::operator+(::std::chrono::nanoseconds rhs) {
    _target.on_advance(rhs);
    return *this;
};
ptrdiff_t iterator::operator-(iterator rhs) const {
    return _target.on_count(rhs._target._timestamp);
};

template<typename... Args>
iterator::iterator(Args &&...args)
    : _target{ ::std::move<Args>(args)... } {

};

sqlite::sqlite(::std::string_view filename)
    : _chat{} {
    _chat._filename = filename;
    _chat.on_init();
};

iterator sqlite::begin() {
    iterator pos{ _chat._database };
    pos._target._timestamp = ::std::numeric_limits<::std::chrono::nanoseconds>::min();
    pos._target._s_message_tag = _chat._s_message_tag;
    pos._target._s_tag_name = _chat._s_tag_name;
    pos._target._s_tag_value = _chat._s_tag_value;
    pos._target._s_count = _chat._s_count;
    pos._target.on_init();
    pos._target.on_advance();
    return pos;
};
iterator sqlite::end() {
    iterator pos{ _chat._database };
    pos._target._timestamp = ::std::numeric_limits<::std::chrono::nanoseconds>::max();
    pos._target._s_count = _chat._s_count;
    return pos;
};

iterator sqlite::insert(message const &message) {
    ::opentelemetry::nostd::shared_ptr<::opentelemetry::trace::Span> span{
        _chat._tracer->StartSpan("insert")
    };
    auto now = ::std::chrono::steady_clock::now();
    iterator pos{ _chat._database };
    pos._target._exceptions = ::std::uncaught_exceptions();
    pos._target._timestamp = now.time_since_epoch();
    ::std::tie(pos._target._commit, pos._target._rollback) = _chat.on_insert_begin(
        span);
    _chat.on_insert_message(pos._target._timestamp, message.content,
        span);
    for (tag const &tag : message.tags) {
        _chat.on_insert_message_tag(pos._target._timestamp, tag.name, tag.value,
            span);
    }
    pos._target._s_message_tag = _chat._s_message_tag;
    pos._target._s_tag_name = _chat._s_tag_name;
    pos._target._s_tag_value = _chat._s_tag_value;
    pos._target._s_count = _chat._s_count;
    pos._target.on_init();
    pos._target.on_advance();
    return pos;
};
iterator sqlite::erase(iterator first, iterator last) {
    ::opentelemetry::nostd::shared_ptr<::opentelemetry::trace::Span> span{
        _chat._tracer->StartSpan("erase")
    };
    iterator pos{ _chat._database };
    pos._target._exceptions = ::std::uncaught_exceptions();
    pos._target._timestamp = last._target._timestamp;
    ::std::tie(pos._target._commit, pos._target._rollback) = _chat.on_erase_begin(
        span);
    _chat.on_erase_message_tag(first._target._timestamp, last._target._timestamp,
        span);
    _chat.on_erase_message(first._target._timestamp, last._target._timestamp,
        span);
    pos._target._s_message_tag = _chat._s_message_tag;
    pos._target._s_tag_name = _chat._s_tag_name;
    pos._target._s_tag_value = _chat._s_tag_value;
    pos._target._s_count = _chat._s_count;
    pos._target.on_init();
    pos._target.on_advance();
    return pos;
};

} // histories
} // chat
} // ai

#endif
