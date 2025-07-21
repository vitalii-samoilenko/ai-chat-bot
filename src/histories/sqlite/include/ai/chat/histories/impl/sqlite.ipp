#ifndef AI_CHAT_HISTORIES_SQLITE_IPP
#define AI_CHAT_HISTORIES_SQLITE_IPP

#include <limits>
#include <utility>

namespace ai {
namespace chat {
namespace histories {

iterator::iterator(iterator const &other)
    : _target{ other._target._database } {
    // other._target.on_upgrade(detail::scope::state::create);
    // _target.on_upgrade(detail::scope::state::create);
    _target._s_message_content = other._target._s_message_content;
    _target._s_message_tag = other._target._s_message_tag;
    _target._s_tag_name = other._target._s_tag_name;
    _target._s_tag_value = other._target._s_tag_value;
    _target._s_count = other._target._s_count;
    _target._s_tag_name_id = other._target._s_tag_name_id;
    _target._s_tag_value_id = other._target._s_tag_value_id;
    _target._u_begin = other._target._u_begin;
    _target._u_message_content = other._target._u_message_content;
    _target._offset = other._target._offset;
    _target._name_ids = other._target._name_ids;
    _target._value_ids = other._target._value_ids;    
    _target._timestamp = other._target._timestamp;
}
iterator::iterator(iterator &&other)
    : _target{ other._target._database } {
    // other._target.on_upgrade(detail::scope::state::create);
    // _target.on_upgrade(*);
    _target._commit = other._target._commit;
    _target._rollback = other._target._rollback;
    _target._s_message = other._target._s_message;
    _target._s_message_content = other._target._s_message_content;
    _target._s_message_tag = other._target._s_message_tag;
    _target._s_tag_name = other._target._s_tag_name;
    _target._s_tag_value = other._target._s_tag_value;
    _target._s_count = other._target._s_count;
    _target._s_tag_name_id = other._target._s_tag_name_id;
    _target._s_tag_value_id = other._target._s_tag_value_id;
    _target._u_begin = other._target._u_begin;
    _target._u_message_content = other._target._u_message_content;
    _target._state = other._target._state;
    _target._exceptions = other._target._exceptions;
    _target._offset = other._target._offset;
    _target._name_ids = ::std::move(other._target._name_ids);
    _target._value_ids = ::std::move(other._target._value_ids);
    _target._timestamp = other._target._timestamp;
    _target._content = ::std::move(other._target._content);
    _target._tag_names = ::std::move(other._target._tag_names);
    _target._tag_values = ::std::move(other._target._tag_values);
    _target._tags = ::std::move(other._target._tags);
    other._target._commit = nullptr;
    other._target._rollback = nullptr;
    other._target._s_message = nullptr;
};

iterator::~iterator() {
    // _target.on_upgrade(detail::scope::state::create);
    if (!_target._commit) {
        return;
    }
    if (!(_target._exceptions == ::std::uncaught_exceptions())) {
        _target.on_rollback();
    }
    _target.on_commit();
};

message iterator::operator*() {
    _target.on_upgrade(detail::scope::state::data);
    return message{
        _target._timestamp,
        _target._content,
        _target._tags
    };
};
iterator & iterator::operator++() {
    _target.on_upgrade(detail::scope::state::cursor);
    _target.on_advance();
    return *this;
};
bool iterator::operator==(iterator const &rhs) const {
    _target.on_upgrade(detail::scope::state::cursor);
    rhs._target.on_upgrade(detail::scope::state::cursor);
    return _target._database == rhs._target._database
        && _target._timestamp == rhs._target._timestamp;
};

iterator iterator::operator+(ptrdiff_t rhs) const {
    // _target.on_upgrade(detail::scope::state::create);
    iterator pos{ *this };
    pos._target.on_advance(rhs);
    return pos;
};
iterator iterator::operator+(::std::chrono::nanoseconds rhs) const {
    _target.on_upgrade(detail::scope::state::cursor);
    iterator pos{ *this };
    pos._target.on_advance(rhs);
    return pos;
};
ptrdiff_t iterator::operator-(iterator rhs) const {
    _target.on_upgrade(detail::scope::state::cursor);
    rhs._target.on_upgrade(detail::scope::state::cursor);
    return _target.on_count(rhs._target._timestamp);
};

iterator & iterator::operator&=(tag rhs) {
    // _target.on_upgrade(detail::scope::state::create);
    _target.on_filter(rhs);
    return *this;
};

iterator & iterator::operator=(::std::string_view rhs) {
    _target.on_upgrade(detail::scope::state::cursor);
    _target.on_update_begin();
    _target.on_update(rhs);
    return *this;
};

bool operator<(iterator const &lhs, ::std::chrono::nanoseconds rhs) {
    lhs._target.on_upgrade(detail::scope::state::cursor);
    return lhs._target._timestamp < rhs;
};
bool operator<(::std::chrono::nanoseconds lhs, iterator const &rhs) {
    rhs._target.on_upgrade(detail::scope::state::cursor);
    return lhs < rhs._target._timestamp;
};

template<typename... Args>
iterator::iterator(Args &&...args)
    : _target{ ::std::forward<Args>(args)... } {
    // _target.on_upgrade(detail::scope::state::create);
};

sqlite::sqlite(::std::string_view filename)
    : _chat{} {
    _chat._filename = filename;
    _chat.on_init();
};

iterator sqlite::begin() {
    iterator pos{ _chat._database };
    pos._target._s_message_content = _chat._s_message_content;
    pos._target._s_message_tag = _chat._s_message_tag;
    pos._target._s_tag_name = _chat._s_tag_name;
    pos._target._s_tag_value = _chat._s_tag_value;
    pos._target._s_count = _chat._s_count;
    pos._target._s_tag_name_id = _chat._s_tag_name_id;
    pos._target._s_tag_value_id = _chat._s_tag_value_id;
    pos._target._u_begin = _chat._u_begin;
    pos._target._u_message_content = _chat._u_message_content;
    pos._target._timestamp = ::std::numeric_limits<::std::chrono::nanoseconds>::min();
    return pos;
};
iterator sqlite::end() {
    iterator pos{ _chat._database };
    pos._target._s_message_content = _chat._s_message_content;
    pos._target._s_message_tag = _chat._s_message_tag;
    pos._target._s_tag_name = _chat._s_tag_name;
    pos._target._s_tag_value = _chat._s_tag_value;
    pos._target._s_count = _chat._s_count;
    pos._target._s_tag_name_id = _chat._s_tag_name_id;
    pos._target._s_tag_value_id = _chat._s_tag_value_id;
    pos._target._u_begin = _chat._u_begin;
    pos._target._u_message_content = _chat._u_message_content;
    pos._target._timestamp = ::std::numeric_limits<::std::chrono::nanoseconds>::max();
    return pos;
};

iterator sqlite::lower_bound(::std::chrono::nanoseconds timestamp) {
    iterator pos{ _chat._database };
    pos._target._s_message_content = _chat._s_message_content;
    pos._target._s_message_tag = _chat._s_message_tag;
    pos._target._s_tag_name = _chat._s_tag_name;
    pos._target._s_tag_value = _chat._s_tag_value;
    pos._target._s_count = _chat._s_count;
    pos._target._s_tag_name_id = _chat._s_tag_name_id;
    pos._target._s_tag_value_id = _chat._s_tag_value_id;
    pos._target._u_begin = _chat._u_begin;
    pos._target._u_message_content = _chat._u_message_content;
    pos._target._timestamp = timestamp;
    return pos;
};

iterator sqlite::insert(message value) {
    ::opentelemetry::nostd::shared_ptr<::opentelemetry::trace::Span> span{
        _chat._tracer->StartSpan("insert")
    };
    auto now = ::std::chrono::utc_clock::now();
    iterator pos{ _chat._database };
    _chat.on_insert_begin(&pos._target._commit, &pos._target._rollback,
        span);
    pos._target._s_message_content = _chat._s_message_content;
    pos._target._s_message_tag = _chat._s_message_tag;
    pos._target._s_tag_name = _chat._s_tag_name;
    pos._target._s_tag_value = _chat._s_tag_value;
    pos._target._s_count = _chat._s_count;
    pos._target._s_tag_name_id = _chat._s_tag_name_id;
    pos._target._s_tag_value_id = _chat._s_tag_value_id;
    pos._target._u_begin = _chat._u_begin;
    pos._target._u_message_content = _chat._u_message_content;
    pos._target._exceptions = ::std::uncaught_exceptions();
    pos._target._timestamp = now.time_since_epoch();
    _chat.on_insert_message(pos._target._timestamp, value.content,
        span);
    for (tag const &tag : value.tags) {
        _chat.on_insert_message_tag(pos._target._timestamp, tag.name, tag.value,
            span);
    }
    return pos;
};
iterator sqlite::erase(iterator pos) {
    ::opentelemetry::nostd::shared_ptr<::opentelemetry::trace::Span> span{
        _chat._tracer->StartSpan("erase")
    };
    pos._target.on_upgrade(detail::scope::state::cursor);
    iterator next{ _chat._database };
    _chat.on_erase_begin(&next._target._commit, &next._target._rollback,
        span);
    next._target._s_message_content = _chat._s_message_content;
    next._target._s_message_tag = _chat._s_message_tag;
    next._target._s_tag_name = _chat._s_tag_name;
    next._target._s_tag_value = _chat._s_tag_value;
    next._target._s_count = _chat._s_count;
    next._target._s_tag_name_id = _chat._s_tag_name_id;
    next._target._s_tag_value_id = _chat._s_tag_value_id;
    next._target._u_begin = _chat._u_begin;
    next._target._u_message_content = _chat._u_message_content;
    next._target._exceptions = ::std::uncaught_exceptions();
    next._target._timestamp = pos._target._timestamp + ::std::chrono::nanoseconds{ 1 };
    _chat.on_erase_message(pos._target._timestamp, next._target._timestamp,
        span);
    return next;
};
iterator sqlite::erase(iterator first, iterator last) {
    ::opentelemetry::nostd::shared_ptr<::opentelemetry::trace::Span> span{
        _chat._tracer->StartSpan("erase")
    };
    first._target.on_upgrade(detail::scope::state::cursor);
    last._target.on_upgrade(detail::scope::state::cursor);
    iterator next{ _chat._database };
    _chat.on_erase_begin(&next._target._commit, &next._target._rollback,
        span);
    next._target._s_message_content = _chat._s_message_content;
    next._target._s_message_tag = _chat._s_message_tag;
    next._target._s_tag_name = _chat._s_tag_name;
    next._target._s_tag_value = _chat._s_tag_value;
    next._target._s_count = _chat._s_count;
    next._target._s_tag_name_id = _chat._s_tag_name_id;
    next._target._s_tag_value_id = _chat._s_tag_value_id;
    next._target._u_begin = _chat._u_begin;
    next._target._u_message_content = _chat._u_message_content;
    next._target._exceptions = ::std::uncaught_exceptions();
    next._target._timestamp = last._target._timestamp;
    _chat.on_erase_message(first._target._timestamp, next._target._timestamp,
        span);
    return next;
};

} // histories
} // chat
} // ai

#endif
