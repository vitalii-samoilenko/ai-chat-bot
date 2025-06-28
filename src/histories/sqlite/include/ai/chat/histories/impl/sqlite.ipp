#ifndef AI_CHAT_HISTORIES_SQLITE_IPP
#define AI_CHAT_HISTORIES_SQLITE_IPP

#include <exception>

#include "boost/scope/scope_fail.hpp"
#include "boost/scope/scope_success.hpp"

#include "sqlite3.h"

#include "ai/chat/histories/sqlite.hpp"

namespace ai {
namespace chat {
namespace histories {

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
        ::sqlite3_finalize(_p_insert_rollback);
        ::sqlite3_finalize(_p_insert_commit);
        ::sqlite3_finalize(_p_insert_tag);
        ::sqlite3_finalize(_p_insert_content);
        ::sqlite3_finalize(_p_insert_begin);
        ::sqlite3_close(_p_database);
    };

private:
    connection()
        : _p_database{ nullptr }
        , _p_insert_begin{ nullptr }
        , _p_insert_content{ nullptr }
        , _p_insert_tag{ nullptr }
        , _p_insert_commit{ nullptr }
        , _p_insert_rollback{ nullptr }
        , _filename{} {

    };

    ::sqlite3* _p_database;
    ::sqlite3_stmt* _p_insert_begin;
    ::sqlite3_stmt* _p_insert_content;
    ::sqlite3_stmt* _p_insert_tag;
    ::sqlite3_stmt* _p_insert_commit;
    ::sqlite3_stmt* _p_insert_rollback;
    ::std::string _filename;

    void ensure_success(int error_code) {
        switch (error_code) {
        case SQLITE_OK:
        case SQLITE_ROW:
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
        ::sqlite3_stmt* _p_init{ nullptr };
        auto on_exit = ::boost::scope::make_scope_exit([this, _p_init]()->void {
            ensure_success(
                ::sqlite3_finalize(_p_init));
        });
        const char INIT[]{
            "CREATE TABLE IF NOT EXISTS ai.message"
            "("
                "timestamp INTEGER NOT NULL CONSTRAINT PK_ai_message PRIMARY KEY,"
                "content TEXT NOT NULL"
            ")"
                "WITHOUT ROWID;"
            "CREATE TABLE IF NOT EXISTS ai.tag_name"
            "("
                "id INTEGER NOT NULL CONSTRAINT PK_ai_tag_name PRIMARY KEY AUTOINCREMENT,"
                "name TEXT NOT NULL CONSTRAINT UQ_ai_tag_name_name UNIQUE"
            ")"
                "WITHOUT ROWID;"
            "CREATE TABLE IF NOT EXISTS ai.tag_value"
            "("
                "id INTEGER NOT NULL CONSTRAINT PK_ai_tag_value PRIMARY KEY AUTOINCREMENT,"
                "name_id INTEGER NOT NULL CONSTRAINT FK_ai_tag_value_ai_tag_name REFERENCES ai.tag_name(id),"
                "value TEXT NOT NULL CONSTRAINT UQ_ai_tag_value_value UNIQUE"
            ")"
                "WITHOUT ROWID;"
            "CREATE TABLE IF NOT EXISTS ai.message_tag"
            "("
                "timestamp INTEGER NOT NULL CONSTRAINT FK_ai_message_tag_ai_message REFERENCES ai.message(timestamp),"
                "value_id INTEGER NOT NULL CONSTRAINT FK_ai_message_tag_ai_tag_value REFERENCES ai.tag_value(id)"
            ")"
                "CONSTRAINT PK_ai_message_tag PRIMARY KEY(timestamp, value_id)"
                "WITHOUT ROWID;"
        };
        ensure_success(
            ::sqlite3_prepare_v2(_p_database, INIT,
                static_cast<int>(::std::size(INIT) - 1),
                &_p_init, nullptr));
        ensure_success(
            ::sqlite3_step(_p_init));
        const char INSERT_BEGIN[]{
            "SAVEPOINT insert_message;"
        };
        ensure_success(
            ::sqlite3_prepare_v2(_p_database, INSERT_BEGIN,
                static_cast<int>(::std::size(INSERT_BEGIN) - 1),
                &_p_insert_begin, nullptr));
        const char INSERT_CONTENT[]{
            "INSERT INTO ai.message"
            "("
                "timestamp, content"
            ")"
            "VALUES"
            "("
                "@TIMESTAMP, @CONTENT"
            ");"
        };
        ensure_success(
            ::sqlite3_prepare_v2(_p_database, INSERT_CONTENT,
                static_cast<int>(::std::size(INSERT_CONTENT) - 1),
                &_p_insert_content, nullptr));
        const char INSERT_TAG[]{
            "INSERT OR IGNORE INTO ai.tag_name"
            "("
                "name"
            ")"
            "VALUES"
            "("
                "@NAME"
            ");"
            "INSERT OR IGNORE INTO ai.tag_value"
            "("
                "name_id, value"
            ")"
            "SELECT id, @VALUE FROM ai.tag_name"
            "WHERE name = @NAME;"
            "INSERT OR IGNORE INTO ai.message_tag"
            "("
                "timestamp, value_id"
            ")"
            "SELECT @TIMESTAMP, value_id FROM ai.tag_value"
            "WHERE value = @VALUE;"
        };
        ensure_success(
            ::sqlite3_prepare_v2(_p_database, INSERT_TAG,
                static_cast<int>(::std::size(INSERT_TAG) - 1),
                &_p_insert_tag, nullptr));
        const char INSERT_COMMIT[]{
            "RELEASE SAVEPOINT insert_message;"
        };
        ensure_success(
            ::sqlite3_prepare_v2(_p_database, INSERT_COMMIT,
                static_cast<int>(::std::size(INSERT_COMMIT) - 1),
                &_p_insert_commit, nullptr));
        const char INSERT_ROLLBACK[]{
            "ROLLBACK TO SAVEPOINT insert_message;"
        };
        ensure_success(
            ::sqlite3_prepare_v2(_p_database, INSERT_ROLLBACK,
                static_cast<int>(::std::size(INSERT_ROLLBACK) - 1),
                &_p_insert_rollback, nullptr));
    };
    iterator_type on_insert(const message_type& message) {
        ::std::chrono::steady_clock::time_point now{ ::std::chrono::steady_clock::now() };
        ensure_success(
            ::sqlite3_step(_p_insert_begin));
        auto on_exit = ::boost::scope::make_scope_exit([this]()->void {
            ensure_success(
                ::sqlite3_step(_p_insert_commit));
        });
        auto on_fail = ::boost::scope::make_scope_fail([this]()->void {
            ensure_success(
                ::sqlite3_step(_p_insert_rollback));
        });
        ensure_success(
            ::sqlite3_bind_int64(_p_insert_content,
                ::sqlite3_bind_parameter_index(_p_insert_content, "@TIMESTAMP"),
                now.time_since_epoch().count()));
        ensure_success(
            ::sqlite3_bind_text(_p_insert_content,
                ::sqlite3_bind_parameter_index(_p_insert_content, "@CONTENT"),
                message.content.c_str(),
                static_cast<int>(message.content.size()),
                SQLITE_STATIC));
        ensure_success(
            ::sqlite3_step(_p_insert_content));
        ensure_success(
            ::sqlite3_reset(_p_insert_content));
        ensure_success(
            ::sqlite3_bind_null(_p_insert_content,
                ::sqlite3_bind_parameter_index(_p_insert_content, "@CONTENT")));
        for (const tag_type& tag : message.tags) {
            ensure_success(
                ::sqlite3_bind_int64(_p_insert_tag,
                    ::sqlite3_bind_parameter_index(_p_insert_tag, "@TIMESTAMP"),
                    now.time_since_epoch().count()));
            ensure_success(
                ::sqlite3_bind_text(_p_insert_tag,
                    ::sqlite3_bind_parameter_index(_p_insert_tag, "@NAME"),
                    tag.name.c_str(),
                    static_cast<int>(tag.name.size()),
                    SQLITE_STATIC));
            ensure_success(
                ::sqlite3_bind_text(_p_insert_tag,
                    ::sqlite3_bind_parameter_index(_p_insert_tag, "@VALUE"),
                    tag.value.c_str(),
                    static_cast<int>(tag.value.size()),
                    SQLITE_STATIC));
            ensure_success(
                ::sqlite3_step(_p_insert_tag));
            ensure_success(
                ::sqlite3_reset(_p_insert_tag));
        }
        ensure_success(
            ::sqlite3_bind_null(_p_insert_tag,
                ::sqlite3_bind_parameter_index(_p_insert_tag, "@NAME")));
        ensure_success(
            ::sqlite3_bind_null(_p_insert_tag,
                ::sqlite3_bind_parameter_index(_p_insert_tag, "@VALUE")));
        return now.time_since_epoch();
    };
};

sqlite::sqlite(const ::std::string& filename)
    : _p_chat{ new connection{} } {
    _p_chat->_filename.append(filename);
    _p_chat->on_init();
};

sqlite::iterator_type sqlite::insert(const message_type& message) {
    return _p_chat->on_insert(message);
};

} // histories
} // chat
} // ai

#endif
