#ifndef AI_CHAT_MODERATORS_DETAIL_CONNECTION_IPP
#define AI_CHAT_MODERATORS_DETAIL_CONNECTION_IPP

#include "esqlite3.hpp"

namespace ai {
namespace chat {
namespace moderators {
namespace detail {

connection::connection()
    : _filename{}
    , _database{ nullptr }
    , _init_wal{ nullptr }
    , _init_user{ nullptr }
    , _init_filter{ nullptr }
    , _is_allowed1{ nullptr }
    , _is_allowed2{ nullptr }
    , _is_filtered{ nullptr }
    , _promote{ nullptr }
    , _demote{ nullptr }
    , _timeout{ nullptr }
    , _filter{ nullptr }
    , _discard{ nullptr }
    INIT_TRACER("ai_chat_moderators_sqlite") {

};

connection::~connection() {
    ::sqlite3_finalize(_discard);
    ::sqlite3_finalize(_filter);
    ::sqlite3_finalize(_timeout);
    ::sqlite3_finalize(_demote);
    ::sqlite3_finalize(_promote);
    ::sqlite3_finalize(_is_filtered);
    ::sqlite3_finalize(_is_allowed2);
    ::sqlite3_finalize(_is_allowed1);
    ::sqlite3_finalize(_init_filter);
    ::sqlite3_finalize(_init_user);
    ::sqlite3_finalize(_init_wal);
    ::sqlite3_close(_database);
};

void connection::on_init() {
    ::esqlite3_ensure_success(
        ::sqlite3_open_v2(_filename.c_str(), &_database,
            SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE,
            nullptr));
    ::esqlite3_ensure_success(
        ::sqlite3_create_function(_database, "regexp", 2,
            SQLITE_UTF8 | SQLITE_DETERMINISTIC | SQLITE_DIRECTONLY | SQLITE_INNOCUOUS,
            nullptr, &::esqlite3_regexp, nullptr, nullptr));
    char const INIT_WAL[]{
        "PRAGMA journal_mode=WAL"
    };
    ::esqlite3_ensure_success(
        ::sqlite3_prepare_v2(_database, INIT_WAL,
            static_cast<int>(::std::size(INIT_WAL) - 1),
            &_init_wal, nullptr));
    ::esqlite3_ensure_success(
        ::sqlite3_step(_init_wal));
    ::esqlite3_ensure_success(
        ::sqlite3_finalize(_init_wal));
    _init_wal = nullptr;
    char const INIT_USER[]{
        "CREATE TABLE IF NOT EXISTS user"
        "("
                "id INTEGER NOT NULL CONSTRAINT PK_user PRIMARY KEY AUTOINCREMENT"
            ", name TEXT NOT NULL CONSTRAINT UQ_user_name UNIQUE"
            ", role INTEGER NOT NULL DEFAULT 0"
            ", since INTEGER NOT NULL DEFAULT 0"
        ")"
    };
    ::esqlite3_ensure_success(
        ::sqlite3_prepare_v2(_database, INIT_USER,
            static_cast<int>(::std::size(INIT_USER) - 1),
            &_init_user, nullptr));
    ::esqlite3_ensure_success(
        ::sqlite3_step(_init_user));
    ::esqlite3_ensure_success(
        ::sqlite3_finalize(_init_user));
    _init_user = nullptr;
    char const INIT_FILTER[]{
        "CREATE TABLE IF NOT EXISTS filter"
        "("
                "id INTEGER NOT NULL CONSTRAINT PK_filter PRIMARY KEY AUTOINCREMENT"
            ", name TEXT NOT NULL CONSTRAINT UQ_filter_name UNIQUE"
            ", pattern TEXT NOT NULL"
        ")"
    };
    ::esqlite3_ensure_success(
        ::sqlite3_prepare_v2(_database, INIT_FILTER,
            static_cast<int>(::std::size(INIT_FILTER) - 1),
            &_init_filter, nullptr));
    ::esqlite3_ensure_success(
        ::sqlite3_step(_init_filter));
    ::esqlite3_ensure_success(
        ::sqlite3_finalize(_init_filter));
    _init_filter = nullptr;
    char const IS_ALLOWED1[]{
        "SELECT COUNT(*) FROM user"
        " WHERE name = @USERNAME1"
        " AND 0 < role & @ROLE"
        " AND NOT @SINCE < since"
    };
    ::esqlite3_ensure_success(
        ::sqlite3_prepare_v2(_database, IS_ALLOWED1,
            static_cast<int>(::std::size(IS_ALLOWED1) - 1),
            &_is_allowed1, nullptr));
    char const IS_ALLOWED2[]{
        "SELECT COUNT(*) FROM user"
        " WHERE name IN (@USERNAME1, @USERNAME2)"
        " AND 0 < role & @ROLE"
        " AND NOT @SINCE < "
        "("
            "SELECT MAX(since) FROM user"
            " WHERE name IN (@USERNAME1, @USERNAME2)"
        ")"
    };
    ::esqlite3_ensure_success(
        ::sqlite3_prepare_v2(_database, IS_ALLOWED2,
            static_cast<int>(::std::size(IS_ALLOWED2) - 1),
            &_is_allowed2, nullptr));
    char const IS_FILTERED[]{
        "SELECT "
        "("
            "SELECT COUNT(*) FROM filter"
            " WHERE @CONTENT REGEXP pattern"
        ")"
    };
    ::esqlite3_ensure_success(
        ::sqlite3_prepare_v2(_database, IS_FILTERED,
            static_cast<int>(::std::size(IS_FILTERED) - 1),
            &_is_filtered, nullptr));
    char const PROMOTE[]{
        "INSERT INTO user"
        "("
            "name, role"
        ")"
        " VALUES"
        "("
            "@USERNAME, @ROLE"
        ")"
        " ON CONFLICT DO UPDATE SET"
            " role = role | @ROLE"
    };
    ::esqlite3_ensure_success(
        ::sqlite3_prepare_v2(_database, PROMOTE,
            static_cast<int>(::std::size(PROMOTE) - 1),
            &_promote, nullptr));
    char const DEMOTE[]{
        "INSERT INTO user"
        "("
            "name, role"
        ")"
        " VALUES"
        "("
            "@USERNAME, @ROLE"
        ")"
        " ON CONFLICT DO UPDATE SET"
            " role = role & ~@ROLE"
    };
    ::esqlite3_ensure_success(
        ::sqlite3_prepare_v2(_database, DEMOTE,
            static_cast<int>(::std::size(DEMOTE) - 1),
            &_demote, nullptr));
    char const TIMEOUT[]{
        "INSERT INTO user"
        "("
            "name, since"
        ")"
        " VALUES"
        "("
            "@USERNAME, @SINCE"
        ")"
        " ON CONFLICT DO UPDATE SET"
            " since = @SINCE"
    };
    ::esqlite3_ensure_success(
        ::sqlite3_prepare_v2(_database, TIMEOUT,
            static_cast<int>(::std::size(TIMEOUT) - 1),
            &_timeout, nullptr));
    char const FILTER[]{
        "INSERT INTO filter"
        "("
            "name, pattern"
        ")"
        " VALUES"
        "("
            "@NAME, @PATTERN"
        ")"
        " ON CONFLICT DO UPDATE SET"
            " pattern = @PATTERN"
    };
    ::esqlite3_ensure_success(
        ::sqlite3_prepare_v2(_database, FILTER,
            static_cast<int>(::std::size(FILTER) - 1),
            &_filter, nullptr));
    char const DISCARD[]{
        "DELETE FROM filter"
        " WHERE name = @NAME"
    };
    ::esqlite3_ensure_success(
        ::sqlite3_prepare_v2(_database, DISCARD,
            static_cast<int>(::std::size(DISCARD) - 1),
            &_discard, nullptr));
};
iterator connection::on_is_allowed(::std::string_view username1, ::sqlite3_int64 role, ::sqlite3_int64 since
    DECLARE_SPAN(root)) {
    START_SUBSPAN(span, "on_is_allowed", root, (*this))
    ::esqlite3_ensure_success(
        ::sqlite3_bind_text(_is_allowed1,
            ::sqlite3_bind_parameter_index(_is_allowed1, "@USERNAME1"),
            username1.data(),
            static_cast<int>(username1.size()),
            SQLITE_STATIC));
    ::esqlite3_ensure_success(
        ::sqlite3_bind_int64(_is_allowed1,
            ::sqlite3_bind_parameter_index(_is_allowed1, "@ROLE"),
            role));
    ::esqlite3_ensure_success(
        ::sqlite3_bind_int64(_is_allowed1,
            ::sqlite3_bind_parameter_index(_is_allowed1, "@SINCE"),
            since));
    ::esqlite3_ensure_success(
        ::sqlite3_step(_is_allowed1));
    iterator count{ static_cast<iterator>(::sqlite3_column_int64(_is_allowed1, 0)) };
    ::esqlite3_ensure_success(
        ::sqlite3_reset(_is_allowed1));
    return count;
};
iterator connection::on_is_allowed(::std::string_view username1, ::std::string_view username2, role role, ::sqlite3_int64 since
    DECLARE_SPAN(root)) {
    START_SUBSPAN(span, "on_is_allowed", root, (*this))
    ::esqlite3_ensure_success(
        ::sqlite3_bind_text(_is_allowed2,
            ::sqlite3_bind_parameter_index(_is_allowed2, "@USERNAME1"),
            username1.data(),
            static_cast<int>(username1.size()),
            SQLITE_STATIC));
    ::esqlite3_ensure_success(
        ::sqlite3_bind_text(_is_allowed2,
            ::sqlite3_bind_parameter_index(_is_allowed2, "@USERNAME2"),
            username2.data(),
            static_cast<int>(username2.size()),
            SQLITE_STATIC));
    ::esqlite3_ensure_success(
        ::sqlite3_bind_int64(_is_allowed2,
            ::sqlite3_bind_parameter_index(_is_allowed2, "@ROLE"),
            role));
    ::esqlite3_ensure_success(
        ::sqlite3_bind_int64(_is_allowed2,
            ::sqlite3_bind_parameter_index(_is_allowed2, "@SINCE"),
            since));
    ::esqlite3_ensure_success(
        ::sqlite3_step(_is_allowed2));
    iterator count{ static_cast<iterator>(::sqlite3_column_int64(_is_allowed2, 0)) };
    ::esqlite3_ensure_success(
        ::sqlite3_reset(_is_allowed2));
    return count;
};
iterator connection::on_is_filtered(::std::string_view content
    DECLARE_SPAN(root)) {
    START_SUBSPAN(span, "on_is_filtered", root, (*this))
    ::esqlite3_ensure_success(
        ::sqlite3_bind_text(_is_filtered,
            ::sqlite3_bind_parameter_index(_is_filtered, "@CONTENT"),
            content.data(),
            static_cast<int>(content.size()),
            SQLITE_STATIC));
    ::esqlite3_ensure_success(
        ::sqlite3_step(_is_filtered));
    iterator count{ static_cast<iterator>(::sqlite3_column_int64(_is_filtered, 0)) };
    ::esqlite3_ensure_success(
        ::sqlite3_reset(_is_filtered));
    return count;
};
iterator connection::on_promote(::std::string_view username, role role
    DECLARE_SPAN(root)) {
    START_SUBSPAN(span, "on_promote", root, (*this))
    ::esqlite3_ensure_success(
        ::sqlite3_bind_text(_promote,
            ::sqlite3_bind_parameter_index(_promote, "@USERNAME"),
            username.data(),
            static_cast<int>(username.size()),
            SQLITE_STATIC));
    ::esqlite3_ensure_success(
        ::sqlite3_bind_int64(_promote,
            ::sqlite3_bind_parameter_index(_promote, "@ROLE"),
            role));
    ::esqlite3_ensure_success(
        ::sqlite3_step(_promote));
    ::esqlite3_ensure_success(
        ::sqlite3_reset(_promote));
    return 1;
};
iterator connection::on_demote(::std::string_view username, role role
    DECLARE_SPAN(root)) {
    START_SUBSPAN(span, "on_demote", root, (*this))
    ::esqlite3_ensure_success(
        ::sqlite3_bind_text(_demote,
            ::sqlite3_bind_parameter_index(_demote, "@USERNAME"),
            username.data(),
            static_cast<int>(username.size()),
            SQLITE_STATIC));
    ::esqlite3_ensure_success(
        ::sqlite3_bind_int64(_demote,
            ::sqlite3_bind_parameter_index(_demote, "@ROLE"),
            role));
    ::esqlite3_ensure_success(
        ::sqlite3_step(_demote));
    ::esqlite3_ensure_success(
        ::sqlite3_reset(_demote));
    return 1;
};
iterator connection::on_timeout(::std::string_view username, ::sqlite3_int64 since
    DECLARE_SPAN(root)) {
    START_SUBSPAN(span, "on_timeout", root, (*this))
    ::esqlite3_ensure_success(
        ::sqlite3_bind_text(_timeout,
            ::sqlite3_bind_parameter_index(_timeout, "@USERNAME"),
            username.data(),
            static_cast<int>(username.size()),
            SQLITE_STATIC));
    ::esqlite3_ensure_success(
        ::sqlite3_bind_int64(_timeout,
            ::sqlite3_bind_parameter_index(_timeout, "@SINCE"),
            since));
    ::esqlite3_ensure_success(
        ::sqlite3_step(_timeout));
    ::esqlite3_ensure_success(
        ::sqlite3_reset(_timeout));
    return 1;
};
iterator connection::on_filter(::std::string_view name, ::std::string_view pattern
    DECLARE_SPAN(root)) {
    START_SUBSPAN(span, "on_filter", root, (*this))
    ::esqlite3_ensure_success(
        ::sqlite3_bind_text(_filter,
            ::sqlite3_bind_parameter_index(_filter, "@NAME"),
            name.data(),
            static_cast<int>(name.size()),
            SQLITE_STATIC));
    ::esqlite3_ensure_success(
        ::sqlite3_bind_text(_filter,
            ::sqlite3_bind_parameter_index(_filter, "@PATTERN"),
            pattern.data(),
            static_cast<int>(pattern.size()),
            SQLITE_STATIC));
    ::esqlite3_ensure_success(
        ::sqlite3_step(_filter));
    ::esqlite3_ensure_success(
        ::sqlite3_reset(_filter));
    return 1;
};
iterator connection::on_discard(::std::string_view name
    DECLARE_SPAN(root)) {
    START_SUBSPAN(span, "on_discard", root, (*this))
    ::esqlite3_ensure_success(
        ::sqlite3_bind_text(_discard,
            ::sqlite3_bind_parameter_index(_discard, "@NAME"),
            name.data(),
            static_cast<int>(name.size()),
            SQLITE_STATIC));
    ::esqlite3_ensure_success(
        ::sqlite3_step(_discard));
    ::esqlite3_ensure_success(
        ::sqlite3_reset(_discard));
    return 1;
};

} // detail
} // moderators
} // chat
} // ai

#endif
