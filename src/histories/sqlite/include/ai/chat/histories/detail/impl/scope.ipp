#ifndef AI_CHAT_HISTORIES_DETAIL_SCOPE_IPP
#define AI_CHAT_HISTORIES_DETAIL_SCOPE_IPP

#include <limits>
#include <utility>

#include "esqlite3/ensure_success.hpp"

#include "ai/chat/histories/detail/scope.hpp"

namespace ai {
namespace chat {
namespace histories {
namespace detail {

scope::~scope() {
    ::sqlite3_finalize(_s_message);
    ::sqlite3_finalize(_rollback);
    ::sqlite3_finalize(_commit);
};

scope::scope(::sqlite3 *database)
    : _database{ database }
    , _timestamp{}
    , _commit{ nullptr }
    , _rollback{ nullptr }
    , _exceptions{}
    , _s_message{ nullptr }
    , _s_message_tag{ nullptr }
    , _s_tag_name{ nullptr }
    , _s_tag_value{ nullptr }
    , _s_count{ nullptr }
    , _tag_names{}
    , _tag_values{}
    , _content{}
    , _tags{} {

};

void scope::on_init() {
    const char SELECT_MESSAGE[]{
        "SELECT timestamp, content FROM message"
        " WHERE NOT timestamp < @TIMESTAMP"
        " LIMIT -1 OFFSET @SKIP"
    };
    ::esqlite3_ensure_success(
        ::sqlite3_prepare_v2(_database, SELECT_MESSAGE,
            static_cast<int>(::std::size(SELECT_MESSAGE) - 1),
            &_s_message, nullptr));
    ::esqlite3_ensure_success(
        ::sqlite3_bind_int64(_s_message,
            ::sqlite3_bind_parameter_index(_s_message, "@TIMESTAMP"),
            _timestamp.count()));
    ::esqlite3_ensure_success(
        ::sqlite3_bind_int64(_s_message,
            ::sqlite3_bind_parameter_index(_s_message, "@SKIP"),
            0));
};
void scope::on_advance() {
    _tags.clear();
    _content = ::std::string_view{};
    _tag_values.clear();
    _tag_names.clear();
    int error_code{ ::sqlite3_step(_s_message) };
    if (error_code == SQLITE_DONE) {
        _timestamp = ::std::numeric_limits<::std::chrono::nanoseconds>::max();
        return;
    }
    ::esqlite3_ensure_success(
        error_code);
    _timestamp = ::std::chrono::nanoseconds{ ::sqlite3_column_int64(_s_message, 0) };
    _content = ::std::string_view{ reinterpret_cast<const char *>(::sqlite3_column_text(_s_message, 1)) };
    ::esqlite3_ensure_success(
        ::sqlite3_bind_int64(_s_message_tag,
            ::sqlite3_bind_parameter_index(_s_message_tag, "@TIMESTAMP"),
            _timestamp.count()));
    for (error_code = ::sqlite3_step(_s_message_tag); error_code == SQLITE_ROW; error_code = ::sqlite3_step(_s_message_tag)) {
        ::esqlite3_ensure_success(
            ::sqlite3_bind_int64(_s_tag_name,
                ::sqlite3_bind_parameter_index(_s_tag_name, "@ID"),
                ::sqlite3_column_int64(_s_message_tag, 0)));
        ::esqlite3_ensure_success(
            ::sqlite3_step(_s_tag_name));
        _tag_names.emplace_back(reinterpret_cast<const char *>(::sqlite3_column_text(_s_tag_name, 0)));
        ::esqlite3_ensure_success(
            ::sqlite3_reset(_s_tag_name));
        ::esqlite3_ensure_success(
            ::sqlite3_bind_int64(_s_tag_value,
                ::sqlite3_bind_parameter_index(_s_tag_value, "@ID"),
                ::sqlite3_column_int64(_s_message_tag, 1)));
        ::esqlite3_ensure_success(
            ::sqlite3_step(_s_tag_value));
        _tag_values.emplace_back(reinterpret_cast<const char *>(::sqlite3_column_text(_s_tag_value, 0)));
        ::esqlite3_ensure_success(
            ::sqlite3_reset(_s_tag_value));
        _tags.emplace_back(_tag_names.back(), _tag_values.back());
    }
    ::esqlite3_ensure_success(
        error_code);
    ::esqlite3_ensure_success(
        ::sqlite3_reset(_s_message_tag));
};
void scope::on_commit() {
    ::esqlite3_ensure_success(
        ::sqlite3_step(_commit));
};
void scope::on_rollback() {
    ::esqlite3_ensure_success(
        ::sqlite3_step(_rollback));
};
void scope::on_advance(ptrdiff_t dist) {
    ::esqlite3_ensure_success(
        ::sqlite3_reset(_s_message));
    ::esqlite3_ensure_success(
        ::sqlite3_bind_int64(_s_message,
            ::sqlite3_bind_parameter_index(_s_message, "@TIMESTAMP"),
            _timestamp.count()));
    ::esqlite3_ensure_success(
        ::sqlite3_bind_int64(_s_message,
            ::sqlite3_bind_parameter_index(_s_message, "@SKIP"),
            dist));
    on_advance();
};
void scope::on_advance(::std::chrono::nanoseconds dist) {
    ::esqlite3_ensure_success(
        ::sqlite3_reset(_s_message));
    ::esqlite3_ensure_success(
        ::sqlite3_bind_int64(_s_message,
            ::sqlite3_bind_parameter_index(_s_message, "@TIMESTAMP"),
            (_timestamp + dist).count()));
    ::esqlite3_ensure_success(
        ::sqlite3_bind_int64(_s_message,
            ::sqlite3_bind_parameter_index(_s_message, "@SKIP"),
            0));
    on_advance();
};
ptrdiff_t scope::on_count(::std::chrono::nanoseconds last) const {
    ::std::chrono::nanoseconds first{ _timestamp };
    if (last < first) {
        ::std::swap(first, last);
    }
    ::esqlite3_ensure_success(
        ::sqlite3_bind_int64(_s_count,
            ::sqlite3_bind_parameter_index(_s_count, "@FIRST"),
            first.count()));
    ::esqlite3_ensure_success(
        ::sqlite3_bind_int64(_s_count,
            ::sqlite3_bind_parameter_index(_s_count, "@LAST"),
            last.count()));
    ::esqlite3_ensure_success(
        ::sqlite3_step(_s_count));
    ptrdiff_t count{ ::sqlite3_column_int64(_s_count, 0) };
    ::esqlite3_ensure_success(
        ::sqlite3_reset(_s_count));
    return _timestamp == first
        ? count
        : -count;
};

} // detail
} // histories
} // chat
} // ai

#endif
