#ifndef AI_CHAT_HISTORIES_DETAIL_SCOPE_IPP
#define AI_CHAT_HISTORIES_DETAIL_SCOPE_IPP

#include <limits>

#include "esqlite3.hpp"

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
    , _commit{ nullptr }
    , _rollback{ nullptr }
    , _s_message{ nullptr }
    , _s_message_content{ nullptr }
    , _s_message_tag{ nullptr }
    , _s_tag_name{ nullptr }
    , _s_tag_value{ nullptr }
    , _s_count{ nullptr }
    , _s_tag_name_id{ nullptr }
    , _s_tag_value_id{ nullptr }
    , _state{ state::create }
    , _exceptions{}
    , _offset{}
    , _name_ids{}
    , _value_ids{}
    , _timestamp{}
    , _content{}
    , _tag_names{}
    , _tag_values{}
    , _tags{} {

};

void scope::on_commit() {
    ::esqlite3_ensure_success(
        ::sqlite3_step(_commit));
};
void scope::on_rollback() {
    ::esqlite3_ensure_success(
        ::sqlite3_step(_rollback));
};
void scope::on_upgrade(state to) const {
    switch (_state) {
    case state::create:
        if (!(to < state::init)) {
            break;
        }
    {
        char const SELECT[] {
            "SELECT timestamp FROM message"
            " WHERE NOT timestamp < @TIMESTAMP"
        };
        char const FILTER_BEGIN[] {
            " AND "
            "("
            "   (name_id IS NULL AND value_id IS NULL)"
        };
        char const FILTER[] {
                " OR (name_id = ? AND value_id = ?)"
        };
        char const FILTER_END[] {
            ")"
        };
        char const GROUP[] {
            " GROUP BY timestamp"
            " HAVING COUNT(*) = 1 + @COUNT"
        };
        char const LIMIT[] {
            " LIMIT -1 OFFSET @OFFSET"
        };
        ::std::string statement{};
        statement.reserve(
            ::std::size(SELECT) -1
            + ::std::size(FILTER_BEGIN) -1
            + _name_ids.size() * (::std::size(FILTER) - 1)
            + ::std::size(FILTER_END) -1
            + ::std::size(GROUP) - 1
            + ::std::size(LIMIT) - 1
        );
        statement.append(SELECT);
        statement.append(FILTER_BEGIN);
        for (size_t i{ 0 }; i < _name_ids.size(); ++i) {
            statement.append(FILTER);
        }
        statement.append(FILTER_END);
        statement.append(GROUP);
        statement.append(LIMIT);
        ::esqlite3_ensure_success(
            ::sqlite3_prepare_v2(_database, statement.data(),
                statement.size(),
                &_s_message, nullptr));
        int p_i{ ::sqlite3_bind_parameter_index(_s_message, "@TIMESTAMP") };
        for (size_t i{ 0 }; i < _name_ids.size(); ++i) {
            ::esqlite3_ensure_success(
                ::sqlite3_bind_int64(_s_message,
                    ++p_i,
                    _name_ids[i]));
            ::esqlite3_ensure_success(
                ::sqlite3_bind_int64(_s_message,
                    ++p_i,
                    _value_ids[i]));
        }
        ::esqlite3_ensure_success(
            ::sqlite3_bind_int64(_s_message,
                ::sqlite3_bind_parameter_index(_s_message, "@COUNT"),
                static_cast<::sqlite3_int64>(_name_ids.size())));
    }
    case state::init:
        if (!(to < state::cursor)) {
            break;
        }
    {
        ::esqlite3_ensure_success(
            ::sqlite3_bind_int64(_s_message,
                ::sqlite3_bind_parameter_index(_s_message, "@TIMESTAMP"),
                _timestamp.count()));
        ::esqlite3_ensure_success(
            ::sqlite3_bind_int64(_s_message,
                ::sqlite3_bind_parameter_index(_s_message, "@OFFSET"),
                _offset));
        int error_code{ ::sqlite3_step(_s_message) };
        if (error_code == SQLITE_DONE) {
            _timestamp = ::std::numeric_limits<::std::chrono::nanoseconds>::max();
        } else {
            ::esqlite3_ensure_success(
                error_code);
            _timestamp = ::std::chrono::nanoseconds{ ::sqlite3_column_int64(_s_message, 0) };
        }
        _offset = 0;
    }
    case state::cursor:
        if (!(to < state::data)) {
            break;
        }
    {
        ::esqlite3_ensure_success(
            ::sqlite3_bind_int64(_s_message_content,
                ::sqlite3_bind_parameter_index(_s_message_content, "@TIMESTAMP"),
                _timestamp.count()));
        ::esqlite3_ensure_success(
            ::sqlite3_step(_s_message_content));
        ::sqlite3_value *value{ ::sqlite3_column_value(_s_message_content, 0) };
        _content = ::std::string{
            reinterpret_cast<char const *>(::sqlite3_value_text(value)),
            static_cast<size_t>(::sqlite3_value_bytes(value))
        };
        ::esqlite3_ensure_success(
            ::sqlite3_reset(_s_message_content));
        _tags.clear();
        _tag_values.clear();
        _tag_names.clear();
        ::esqlite3_ensure_success(
            ::sqlite3_bind_int64(_s_message_tag,
                ::sqlite3_bind_parameter_index(_s_message_tag, "@TIMESTAMP"),
                _timestamp.count()));
        int error_code{ ::sqlite3_step(_s_message_tag) };
        for (; error_code == SQLITE_ROW; error_code = ::sqlite3_step(_s_message_tag)) {
            ::esqlite3_ensure_success(
                ::sqlite3_bind_int64(_s_tag_name,
                    ::sqlite3_bind_parameter_index(_s_tag_name, "@ID"),
                    ::sqlite3_column_int64(_s_message_tag, 0)));
            ::esqlite3_ensure_success(
                ::sqlite3_step(_s_tag_name));
            value = ::sqlite3_column_value(_s_tag_name, 0);
            _tag_names.emplace_back(
                reinterpret_cast<char const *>(::sqlite3_value_text(value)),
                static_cast<size_t>(::sqlite3_value_bytes(value)));
            ::esqlite3_ensure_success(
                ::sqlite3_reset(_s_tag_name));
            ::esqlite3_ensure_success(
                ::sqlite3_bind_int64(_s_tag_value,
                    ::sqlite3_bind_parameter_index(_s_tag_value, "@ID"),
                    ::sqlite3_column_int64(_s_message_tag, 1)));
            ::esqlite3_ensure_success(
                ::sqlite3_step(_s_tag_value));
            value = ::sqlite3_column_value(_s_tag_value, 0);
            _tag_values.emplace_back(
                reinterpret_cast<char const *>(::sqlite3_value_text(value)),
                static_cast<size_t>(::sqlite3_value_bytes(value)));
            ::esqlite3_ensure_success(
                ::sqlite3_reset(_s_tag_value));
        }
        ::esqlite3_ensure_success(
            error_code);
        ::esqlite3_ensure_success(
            ::sqlite3_reset(_s_message_tag));
        for (size_t i{ 0 }; i < _tag_names.size(); ++i) {
            _tags.emplace_back(_tag_names[i], _tag_values[i]);
        }
    }
    }
    _state = to;
};
void scope::on_advance() {
    _state = state::cursor;
    int error_code{ ::sqlite3_step(_s_message) };
    if (error_code == SQLITE_DONE) {
        _timestamp = ::std::numeric_limits<::std::chrono::nanoseconds>::max();
    } else {
        ::esqlite3_ensure_success(
            error_code);
        _timestamp = ::std::chrono::nanoseconds{ ::sqlite3_column_int64(_s_message, 0) };
    }
};
void scope::on_advance(ptrdiff_t dist) {
    if (state::init < _state) {
        ::esqlite3_ensure_success(
            ::sqlite3_reset(_s_message));
        _state = state::init;
    }
    _offset += dist;
};
void scope::on_advance(::std::chrono::nanoseconds dist) {
    if (state::init < _state) {
        ::esqlite3_ensure_success(
            ::sqlite3_reset(_s_message));
        _state = state::init;
    }
    _timestamp += dist;
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
void scope::on_append(tag filter) {
    if (state::create < _state) {
        ::esqlite3_ensure_success(
            ::sqlite3_finalize(_s_message));
        _s_message = nullptr;
        _state = state::create;
    }
    ::esqlite3_ensure_success(
        ::sqlite3_bind_text(_s_tag_name_id,
            ::sqlite3_bind_parameter_index(_s_tag_name_id, "@NAME"),
            filter.name.data(),
            static_cast<int>(filter.name.size()),
            SQLITE_STATIC));
    int error_code{ ::sqlite3_step(_s_tag_name_id) };
    if (error_code == SQLITE_ROW) {
        _name_ids.push_back(::sqlite3_column_int64(_s_tag_name_id, 0));
    } else {
        ::esqlite3_ensure_success(
            error_code);
        _timestamp = ::std::numeric_limits<::std::chrono::nanoseconds>::max();
    }
    ::esqlite3_ensure_success(
        ::sqlite3_reset(_s_tag_name_id));
    if (!(error_code == SQLITE_ROW)) {
        return;
    }
    ::esqlite3_ensure_success(
        ::sqlite3_bind_text(_s_tag_value_id,
            ::sqlite3_bind_parameter_index(_s_tag_value_id, "@VALUE"),
            filter.value.data(),
            static_cast<int>(filter.value.size()),
            SQLITE_STATIC));
    error_code = ::sqlite3_step(_s_tag_value_id);
    if (error_code == SQLITE_ROW) {
        _value_ids.push_back(::sqlite3_column_int64(_s_tag_value_id, 0));
    } else {
        _name_ids.pop_back();
        ::esqlite3_ensure_success(
            error_code);
        _timestamp = ::std::numeric_limits<::std::chrono::nanoseconds>::max();
    }
    ::esqlite3_ensure_success(
        ::sqlite3_reset(_s_tag_value_id));
};

} // detail
} // histories
} // chat
} // ai

#endif
