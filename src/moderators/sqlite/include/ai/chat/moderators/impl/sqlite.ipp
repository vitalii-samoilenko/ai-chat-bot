#ifndef AI_CHAT_MODERATORS_SQLITE_IPP
#define AI_CHAT_MODERATORS_SQLITE_IPP

#include <exception>
#include <limits>
#include <regex>

#include "boost/scope/scope_fail.hpp"
#include "boost/scope/scope_success.hpp"

#include "sqlite3.h"

#include "ai/chat/moderators/sqlite.hpp"

extern "C" {
    void sqlite3_regexp(::sqlite3_context* context, int argc, ::sqlite3_value* argv[]) {
        const char* pattern_begin{ reinterpret_cast<const char*>(::sqlite3_value_text(argv[0])) };
        const char* pattern_end{ pattern_begin + ::sqlite3_value_bytes(argv[0]) };
        const char* value_begin{ reinterpret_cast<const char*>(::sqlite3_value_text(argv[1])) };
        const char* value_end{ value_begin + ::sqlite3_value_bytes(argv[1]) };
        ::std::regex pattern{ pattern_begin, pattern_end };
        ::sqlite3_result_int(context, ::std::regex_match(value_begin, value_end, pattern));
    };
};

namespace ai {
namespace chat {
namespace moderators {

class sqlite::connection {
    friend sqlite;

public:
    // connection() = delete;
    connection(const connection&) = delete;
    connection(connection&&) = delete;

    // ~connection() = default;

    connection& operator=(const connection&) = delete;
    connection& operator=(connection&&) = delete;

    ~connection() {
        ::sqlite3_finalize(_p_discard);
        ::sqlite3_finalize(_p_filter);
        ::sqlite3_finalize(_p_timeout);
        ::sqlite3_finalize(_p_demote);
        ::sqlite3_finalize(_p_promote);
        ::sqlite3_finalize(_p_is_filtered);
        ::sqlite3_finalize(_p_is_allowed2);
        ::sqlite3_finalize(_p_is_allowed1);
        ::sqlite3_close(_p_database);
    };

private:
    enum role : ::sqlite3_int64 {
        none         = 0b000000000000000000000000000000000000000000000000000000000000,
        moderator    = 0b000000000000000000000000000000000000000000000000000000000001,
        interlocutor = 0b000000000000000000000000000000000000000000000000000000000010
    };

    connection()
        : _p_database{ nullptr }
        , _p_is_allowed1{ nullptr }
        , _p_is_allowed2{ nullptr }
        , _p_is_filtered{ nullptr }
        , _p_promote{ nullptr }
        , _p_demote{ nullptr }
        , _p_timeout{ nullptr }
        , _p_filter{ nullptr }
        , _p_discard{ nullptr }
        , _filename{} {

    };

    ::sqlite3* _p_database;
    ::sqlite3_stmt* _p_is_allowed1;
    ::sqlite3_stmt* _p_is_allowed2;
    ::sqlite3_stmt* _p_is_filtered;
    ::sqlite3_stmt* _p_promote;
    ::sqlite3_stmt* _p_demote;
    ::sqlite3_stmt* _p_timeout;
    ::sqlite3_stmt* _p_filter;
    ::sqlite3_stmt* _p_discard;
    ::std::string _filename;

    void ensure_success(int error_code) {
        switch (error_code) {
        case SQLITE_OK:
        case SQLITE_ROW:
        case SQLITE_DONE:
            return;
        default:
            throw ::std::exception{ ::sqlite3_errstr(error_code) };
        }
    };
    void on_init() {
        ensure_success(
            ::sqlite3_open_v2(_filename.c_str(), &_p_database,
                SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE,
                nullptr));
        ensure_success(
            ::sqlite3_create_function(_p_database, "regexp", 2,
                SQLITE_UTF8 | SQLITE_DETERMINISTIC | SQLITE_DIRECTONLY | SQLITE_INNOCUOUS,
                nullptr, &::sqlite3_regexp, nullptr, nullptr));
        ::sqlite3_stmt* _p_init_user{ nullptr };
        ::sqlite3_stmt* _p_init_filter{ nullptr };
        auto on_exit = ::boost::scope::make_scope_exit([=]()->void {
            ensure_success(
                ::sqlite3_finalize(_p_init_user));
            ensure_success(
                ::sqlite3_finalize(_p_init_filter));
        });
        const char INIT_USER[]{
            "CREATE TABLE IF NOT EXISTS user"
            "("
                  "id INTEGER NOT NULL CONSTRAINT PK_user PRIMARY KEY AUTOINCREMENT"
                ", name TEXT NOT NULL CONSTRAINT UQ_user_name UNIQUE"
                ", role INTEGER NOT NULL DEFAULT 0"
                ", since INTEGER NOT NULL DEFAULT 0"
            ")"
        };
        ensure_success(
            ::sqlite3_prepare_v2(_p_database, INIT_USER,
                static_cast<int>(::std::size(INIT_USER) - 1),
                &_p_init_user, nullptr));
        ensure_success(
            ::sqlite3_step(_p_init_user));
        const char INIT_FILTER[]{
            "CREATE TABLE IF NOT EXISTS filter"
            "("
                  "id INTEGER NOT NULL CONSTRAINT PK_filter PRIMARY KEY AUTOINCREMENT"
                ", name TEXT NOT NULL CONSTRAINT UQ_filter_name UNIQUE"
                ", pattern TEXT NOT NULL"
            ")"
        };
        ensure_success(
            ::sqlite3_prepare_v2(_p_database, INIT_FILTER,
                static_cast<int>(::std::size(INIT_FILTER) - 1),
                &_p_init_filter, nullptr));
        ensure_success(
            ::sqlite3_step(_p_init_filter));
        const char IS_ALLOWED1[]{
            "SELECT COUNT(*) FROM user"
            " WHERE name = @USERNAME1"
            " AND 0 < role & @ROLE"
            " AND NOT @SINCE < since"
        };
        ensure_success(
            ::sqlite3_prepare_v2(_p_database, IS_ALLOWED1,
                static_cast<int>(::std::size(IS_ALLOWED1) - 1),
                &_p_is_allowed1, nullptr));
        const char IS_ALLOWED2[]{
            "SELECT COUNT(*) FROM user"
            " WHERE name IN (@USERNAME1, @USERNAME2)"
            " AND 0 < role & @ROLE"
            " AND NOT @SINCE < "
            "("
                "SELECT MAX(since) FROM user"
                " WHERE name IN (@USERNAME1, @USERNAME2)"
            ")"
        };
        ensure_success(
            ::sqlite3_prepare_v2(_p_database, IS_ALLOWED2,
                static_cast<int>(::std::size(IS_ALLOWED2) - 1),
                &_p_is_allowed2, nullptr));
        const char IS_FILTERED[]{
            "SELECT COUNT(*) FROM filter"
            " WHERE @CONTENT REGEXP pattern"
        };
        ensure_success(
            ::sqlite3_prepare_v2(_p_database, IS_FILTERED,
                static_cast<int>(::std::size(IS_FILTERED) - 1),
                &_p_is_filtered, nullptr));
        const char PROMOTE[]{
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
        ensure_success(
            ::sqlite3_prepare_v2(_p_database, PROMOTE,
                static_cast<int>(::std::size(PROMOTE) - 1),
                &_p_promote, nullptr));
        const char DEMOTE[]{
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
        ensure_success(
            ::sqlite3_prepare_v2(_p_database, DEMOTE,
                static_cast<int>(::std::size(DEMOTE) - 1),
                &_p_demote, nullptr));
        const char TIMEOUT[]{
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
        ensure_success(
            ::sqlite3_prepare_v2(_p_database, TIMEOUT,
                static_cast<int>(::std::size(TIMEOUT) - 1),
                &_p_timeout, nullptr));
        const char FILTER[]{
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
        ensure_success(
            ::sqlite3_prepare_v2(_p_database, FILTER,
                static_cast<int>(::std::size(FILTER) - 1),
                &_p_filter, nullptr));
        const char DISCARD[]{
            "DELETE FROM filter"
            " WHERE name = @NAME"
        };
        ensure_success(
            ::sqlite3_prepare_v2(_p_database, DISCARD,
                static_cast<int>(::std::size(DISCARD) - 1),
                &_p_discard, nullptr));
    };
    iterator on_is_allowed(const ::std::string& username1, role role, ::sqlite3_int64 since) {
        ensure_success(
            ::sqlite3_bind_text(_p_is_allowed1,
                ::sqlite3_bind_parameter_index(_p_is_allowed1, "@USERNAME1"),
                username1.c_str(),
                static_cast<int>(username1.size()),
                SQLITE_STATIC));
        ensure_success(
            ::sqlite3_bind_int64(_p_is_allowed1,
                ::sqlite3_bind_parameter_index(_p_is_allowed1, "@ROLE"),
                role::moderator));
        ensure_success(
            ::sqlite3_bind_int64(_p_is_allowed1,
                ::sqlite3_bind_parameter_index(_p_is_allowed1, "@SINCE"),
                since));
        ensure_success(
            ::sqlite3_step(_p_is_allowed1));
        iterator count{ static_cast<iterator>(::sqlite3_column_int64(_p_is_allowed1, 0)) };
        ensure_success(
            ::sqlite3_reset(_p_is_allowed1));
        return count;
    };
    iterator on_is_allowed(const ::std::string& username1, const ::std::string& username2, role role, ::sqlite3_int64 since) {
        ensure_success(
            ::sqlite3_bind_text(_p_is_allowed2,
                ::sqlite3_bind_parameter_index(_p_is_allowed2, "@USERNAME1"),
                username1.c_str(),
                static_cast<int>(username1.size()),
                SQLITE_STATIC));
        ensure_success(
            ::sqlite3_bind_text(_p_is_allowed2,
                ::sqlite3_bind_parameter_index(_p_is_allowed2, "@USERNAME2"),
                username2.c_str(),
                static_cast<int>(username2.size()),
                SQLITE_STATIC));
        ensure_success(
            ::sqlite3_bind_int64(_p_is_allowed2,
                ::sqlite3_bind_parameter_index(_p_is_allowed2, "@ROLE"),
                role));
        ensure_success(
            ::sqlite3_bind_int64(_p_is_allowed2,
                ::sqlite3_bind_parameter_index(_p_is_allowed2, "@SINCE"),
                since));
        ensure_success(
            ::sqlite3_step(_p_is_allowed2));
        iterator count{ static_cast<iterator>(::sqlite3_column_int64(_p_is_allowed2, 0)) };
        ensure_success(
            ::sqlite3_reset(_p_is_allowed2));
        return count;
    };
    iterator on_is_filtered(const ::std::string& content) {
        ensure_success(
            ::sqlite3_bind_text(_p_is_filtered,
                ::sqlite3_bind_parameter_index(_p_is_filtered, "@CONTENT"),
                content.c_str(),
                static_cast<int>(content.size()),
                SQLITE_STATIC));
        ensure_success(
            ::sqlite3_step(_p_is_filtered));
        iterator count{ static_cast<iterator>(::sqlite3_column_int64(_p_is_filtered, 0)) };
        ensure_success(
            ::sqlite3_reset(_p_is_filtered));
        return count;
    };
    iterator on_promote(const ::std::string& username, role role) {
        ensure_success(
            ::sqlite3_bind_text(_p_promote,
                ::sqlite3_bind_parameter_index(_p_promote, "@USERNAME"),
                username.c_str(),
                static_cast<int>(username.size()),
                SQLITE_STATIC));
        ensure_success(
            ::sqlite3_bind_int64(_p_promote,
                ::sqlite3_bind_parameter_index(_p_promote, "@ROLE"),
                role));
        ensure_success(
            ::sqlite3_step(_p_promote));
        ensure_success(
            ::sqlite3_reset(_p_promote));
        return 1;
    };
    iterator on_demote(const ::std::string& username, role role) {
        ensure_success(
            ::sqlite3_bind_text(_p_demote,
                ::sqlite3_bind_parameter_index(_p_demote, "@USERNAME"),
                username.c_str(),
                static_cast<int>(username.size()),
                SQLITE_STATIC));
        ensure_success(
            ::sqlite3_bind_int64(_p_demote,
                ::sqlite3_bind_parameter_index(_p_demote, "@ROLE"),
                role));
        ensure_success(
            ::sqlite3_step(_p_demote));
        ensure_success(
            ::sqlite3_reset(_p_demote));
        return 1;
    };
    iterator on_timeout(const ::std::string& username, ::sqlite3_int64 since) {
        ensure_success(
            ::sqlite3_bind_text(_p_timeout,
                ::sqlite3_bind_parameter_index(_p_timeout, "@USERNAME"),
                username.c_str(),
                static_cast<int>(username.size()),
                SQLITE_STATIC));
        ensure_success(
            ::sqlite3_bind_int64(_p_timeout,
                ::sqlite3_bind_parameter_index(_p_timeout, "@SINCE"),
                since));
        ensure_success(
            ::sqlite3_step(_p_timeout));
        ensure_success(
            ::sqlite3_reset(_p_timeout));
        return 1;
    };
    iterator on_filter(const ::std::string& name, const ::std::string& pattern) {
        ensure_success(
            ::sqlite3_bind_text(_p_filter,
                ::sqlite3_bind_parameter_index(_p_filter, "@NAME"),
                name.c_str(),
                static_cast<int>(name.size()),
                SQLITE_STATIC));
        ensure_success(
            ::sqlite3_bind_text(_p_filter,
                ::sqlite3_bind_parameter_index(_p_filter, "@PATTERN"),
                pattern.c_str(),
                static_cast<int>(pattern.size()),
                SQLITE_STATIC));
        ensure_success(
            ::sqlite3_step(_p_filter));
        ensure_success(
            ::sqlite3_reset(_p_filter));
        return 1;
    };
    iterator on_discard(const ::std::string& name) {
        ensure_success(
            ::sqlite3_bind_text(_p_discard,
                ::sqlite3_bind_parameter_index(_p_discard, "@NAME"),
                name.c_str(),
                static_cast<int>(name.size()),
                SQLITE_STATIC));
        ensure_success(
            ::sqlite3_step(_p_discard));
        ensure_success(
            ::sqlite3_reset(_p_discard));
        return 1;
    };
};

sqlite::sqlite(const ::std::string& filename)
    : _p_controller{ new connection{} } {
    _p_controller->_filename = filename;
    _p_controller->on_init();
};

sqlite::iterator sqlite::is_moderator(const ::std::string& username) {
    return _p_controller->on_is_allowed(username, connection::role::moderator,
        ::std::numeric_limits<::sqlite3_int64>::max());
};
sqlite::iterator sqlite::is_allowed(const ::std::string& username1, const ::std::string& username2) {
    ::std::chrono::steady_clock::time_point now{ ::std::chrono::steady_clock::now() };
    return _p_controller->on_is_allowed(username1, username2, connection::role::interlocutor,
        ::std::chrono::duration_cast<::std::chrono::seconds>(now.time_since_epoch()).count());
};
sqlite::iterator sqlite::is_filtered(const ::std::string& content) {
    return _p_controller->on_is_filtered(content);
};

sqlite::iterator sqlite::mod(const ::std::string& username) {
    return _p_controller->on_promote(username, connection::role::moderator);
};
sqlite::iterator sqlite::unmod(const ::std::string& username) {
    return _p_controller->on_demote(username, connection::role::moderator);
};
sqlite::iterator sqlite::allow(const ::std::string& username) {
    return _p_controller->on_promote(username, connection::role::interlocutor);
};
sqlite::iterator sqlite::deny(const ::std::string& username) {
    return _p_controller->on_demote(username, connection::role::interlocutor);
};
sqlite::iterator sqlite::timeout(const ::std::string& username, ::std::chrono::seconds until) {
    return _p_controller->on_timeout(username, until.count());
};
sqlite::iterator sqlite::ban(const ::std::string& username) {
    return _p_controller->on_timeout(username, ::std::numeric_limits<::sqlite3_int64>::max());
};
sqlite::iterator sqlite::unban(const ::std::string& username) {
    return _p_controller->on_timeout(username, 0);
};
sqlite::iterator sqlite::filter(const ::std::string& name, const ::std::string& pattern) {
    return _p_controller->on_filter(name, pattern);
};
sqlite::iterator sqlite::discard(const ::std::string& name) {
    return _p_controller->on_discard(name);
};

} // moderators
} // chat
} // ai

#endif
