#ifndef AI_CHAT_HISTORIES_SQLITE_IPP
#define AI_CHAT_HISTORIES_SQLITE_IPP

#include <stdexcept>

#include "boost/scope/scope_fail.hpp"
#include "boost/scope/scope_success.hpp"
#include "sqlite3.h"

#include "ai/chat/histories/sqlite.hpp"

namespace ai {
namespace chat {
namespace histories {

class sqlite::connection {
public:
    // connection() = delete;
    connection(const connection&) = delete;
    connection(connection&&) = delete;

    // ~connection() = default;

    connection& operator=(const connection&) = delete;
    connection& operator=(connection&&) = delete;

    ~connection() {
        ::sqlite3_finalize(_insert_rollback);
        ::sqlite3_finalize(_insert_commit);
        ::sqlite3_finalize(_insert_tag_name);
        ::sqlite3_finalize(_insert_tag_value);
        ::sqlite3_finalize(_insert_message_tag);
        ::sqlite3_finalize(_insert_content);
        ::sqlite3_finalize(_insert_begin);
        ::sqlite3_close(_database);
    };

private:
    friend sqlite;

    connection()
        : _database{ nullptr }
        , _insert_begin{ nullptr }
        , _insert_content{ nullptr }
        , _insert_tag_name{ nullptr }
        , _insert_tag_value{ nullptr }
        , _insert_message_tag{ nullptr }
        , _insert_commit{ nullptr }
        , _insert_rollback{ nullptr }
        , _filename{} {

    };

    ::sqlite3* _database;
    ::sqlite3_stmt* _insert_begin;
    ::sqlite3_stmt* _insert_content;
    ::sqlite3_stmt* _insert_tag_name;
    ::sqlite3_stmt* _insert_tag_value;
    ::sqlite3_stmt* _insert_message_tag;
    ::sqlite3_stmt* _insert_commit;
    ::sqlite3_stmt* _insert_rollback;
    ::std::string _filename;

    void ensure_success(int error_code) {
        switch (error_code) {
        case SQLITE_OK:
        case SQLITE_ROW:
        case SQLITE_DONE:
            return;
        default:
            throw ::std::runtime_error{ ::sqlite3_errstr(error_code) };
        }
    };
    void on_init() {
        ensure_success(
            ::sqlite3_open_v2(_filename.c_str(), &_database,
                SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE,
                nullptr));
        ::sqlite3_stmt* _init_message{ nullptr };
        ::sqlite3_stmt* _init_tag_name{ nullptr };
        ::sqlite3_stmt* _init_tag_value{ nullptr };
        ::sqlite3_stmt* _init_message_tag{ nullptr };
        auto on_exit = ::boost::scope::make_scope_exit([=]()->void {
            ensure_success(
                ::sqlite3_finalize(_init_message));
            ensure_success(
                ::sqlite3_finalize(_init_tag_name));
            ensure_success(
                ::sqlite3_finalize(_init_tag_value));
            ensure_success(
                ::sqlite3_finalize(_init_message_tag));
        });
        const char INIT_MESSAGE[]{
            "CREATE TABLE IF NOT EXISTS message"
            "("
                  "timestamp INTEGER NOT NULL CONSTRAINT PK_message PRIMARY KEY"
                ", content TEXT NOT NULL"
            ") WITHOUT ROWID"
        };
        ensure_success(
            ::sqlite3_prepare_v2(_database, INIT_MESSAGE,
                static_cast<int>(::std::size(INIT_MESSAGE) - 1),
                &_init_message, nullptr));
        ensure_success(
            ::sqlite3_step(_init_message));
        const char INIT_TAG_NAME[]{
            "CREATE TABLE IF NOT EXISTS tag_name"
            "("
                  "id INTEGER NOT NULL CONSTRAINT PK_tag_name PRIMARY KEY AUTOINCREMENT"
                ", name TEXT NOT NULL CONSTRAINT UQ_tag_name_name UNIQUE"
            ")"
        };
        ensure_success(
            ::sqlite3_prepare_v2(_database, INIT_TAG_NAME,
                static_cast<int>(::std::size(INIT_TAG_NAME) - 1),
                &_init_tag_name, nullptr));
        ensure_success(
            ::sqlite3_step(_init_tag_name));
        const char INIT_TAG_VALUE[]{
            "CREATE TABLE IF NOT EXISTS tag_value"
            "("
                  "id INTEGER NOT NULL CONSTRAINT PK_tag_value PRIMARY KEY AUTOINCREMENT"
                ", value TEXT NOT NULL CONSTRAINT UQ_tag_value_value UNIQUE"
            ")"
        };
        ensure_success(
            ::sqlite3_prepare_v2(_database, INIT_TAG_VALUE,
                static_cast<int>(::std::size(INIT_TAG_VALUE) - 1),
                &_init_tag_value, nullptr));
        ensure_success(
            ::sqlite3_step(_init_tag_value));
        const char INIT_MESSAGE_TAG[]{
            "CREATE TABLE IF NOT EXISTS message_tag"
            "("
                  "timestamp INTEGER NOT NULL CONSTRAINT FK_message_tag_message REFERENCES message(timestamp)"
                ", name_id INTEGER NOT NULL CONSTRAINT FK_message_tag_tag_name REFERENCES tag_name(id)"
                ", value_id INTEGER NOT NULL CONSTRAINT FK_message_tag_tag_value REFERENCES tag_value(id)"
                ", CONSTRAINT PK_message_tag PRIMARY KEY(timestamp, name_id, value_id)"
            ") WITHOUT ROWID"
        };
        ensure_success(
            ::sqlite3_prepare_v2(_database, INIT_MESSAGE_TAG,
                static_cast<int>(::std::size(INIT_MESSAGE_TAG) - 1),
                &_init_message_tag, nullptr));
        ensure_success(
            ::sqlite3_step(_init_message_tag));
        const char INSERT_BEGIN[]{
            "SAVEPOINT insert_message"
        };
        ensure_success(
            ::sqlite3_prepare_v2(_database, INSERT_BEGIN,
                static_cast<int>(::std::size(INSERT_BEGIN) - 1),
                &_insert_begin, nullptr));
        const char INSERT_CONTENT[]{
            "INSERT INTO message"
            "("
                "timestamp, content"
            ")"
            " VALUES"
            "("
                "@TIMESTAMP, @CONTENT"
            ")"
        };
        ensure_success(
            ::sqlite3_prepare_v2(_database, INSERT_CONTENT,
                static_cast<int>(::std::size(INSERT_CONTENT) - 1),
                &_insert_content, nullptr));
        const char INSERT_TAG_NAME[]{
            "INSERT OR IGNORE INTO tag_name"
            "("
                "name"
            ")"
            " VALUES"
            "("
                "@NAME"
            ")"
        };
        ensure_success(
            ::sqlite3_prepare_v2(_database, INSERT_TAG_NAME,
                static_cast<int>(::std::size(INSERT_TAG_NAME) - 1),
                &_insert_tag_name, nullptr));
        const char INSERT_TAG_VALUE[]{
            "INSERT OR IGNORE INTO tag_value"
            "("
                "value"
            ")"
            " VALUES"
            "("
                "@VALUE"
            ")"
        };
        ensure_success(
            ::sqlite3_prepare_v2(_database, INSERT_TAG_VALUE,
                static_cast<int>(::std::size(INSERT_TAG_VALUE) - 1),
                &_insert_tag_value, nullptr));
        const char INSERT_MESSAGE_TAG[]{
            "INSERT OR IGNORE INTO message_tag"
            "("
                "timestamp, name_id, value_id"
            ")"
            " SELECT @TIMESTAMP"
            ", (SELECT id FROM tag_name WHERE name = @NAME)"
            ", (SELECT id FROM tag_value WHERE value = @VALUE)"
        };
        ensure_success(
            ::sqlite3_prepare_v2(_database, INSERT_MESSAGE_TAG,
                static_cast<int>(::std::size(INSERT_MESSAGE_TAG) - 1),
                &_insert_message_tag, nullptr));
        const char INSERT_COMMIT[]{
            "RELEASE SAVEPOINT insert_message"
        };
        ensure_success(
            ::sqlite3_prepare_v2(_database, INSERT_COMMIT,
                static_cast<int>(::std::size(INSERT_COMMIT) - 1),
                &_insert_commit, nullptr));
        const char INSERT_ROLLBACK[]{
            "ROLLBACK TO SAVEPOINT insert_message"
        };
        ensure_success(
            ::sqlite3_prepare_v2(_database, INSERT_ROLLBACK,
                static_cast<int>(::std::size(INSERT_ROLLBACK) - 1),
                &_insert_rollback, nullptr));
    };
    iterator on_insert(const message& message) {
        ::std::chrono::steady_clock::time_point now{ ::std::chrono::steady_clock::now() };
        ensure_success(
            ::sqlite3_step(_insert_begin));
        auto on_exit = ::boost::scope::make_scope_exit([this]()->void {
            ensure_success(
                ::sqlite3_step(_insert_commit));
        });
        auto on_fail = ::boost::scope::make_scope_fail([this]()->void {
            ensure_success(
                ::sqlite3_step(_insert_rollback));
        });
        ensure_success(
            ::sqlite3_bind_int64(_insert_content,
                ::sqlite3_bind_parameter_index(_insert_content, "@TIMESTAMP"),
                now.time_since_epoch().count()));
        ensure_success(
            ::sqlite3_bind_text(_insert_content,
                ::sqlite3_bind_parameter_index(_insert_content, "@CONTENT"),
                message.content.c_str(),
                static_cast<int>(message.content.size()),
                SQLITE_STATIC));
        ensure_success(
            ::sqlite3_step(_insert_content));
        ensure_success(
            ::sqlite3_reset(_insert_content));
        for (const tag& tag : message.tags) {
            ensure_success(
                ::sqlite3_bind_text(_insert_tag_name,
                    ::sqlite3_bind_parameter_index(_insert_tag_name, "@NAME"),
                    tag.name.c_str(),
                    static_cast<int>(tag.name.size()),
                    SQLITE_STATIC));
            ensure_success(
                ::sqlite3_step(_insert_tag_name));
            ensure_success(
                ::sqlite3_reset(_insert_tag_name));
            ensure_success(
                ::sqlite3_bind_text(_insert_tag_value,
                    ::sqlite3_bind_parameter_index(_insert_tag_value, "@VALUE"),
                    tag.value.c_str(),
                    static_cast<int>(tag.value.size()),
                    SQLITE_STATIC));
            ensure_success(
                ::sqlite3_step(_insert_tag_value));
            ensure_success(
                ::sqlite3_reset(_insert_tag_value));
            ensure_success(
                ::sqlite3_bind_int64(_insert_message_tag,
                    ::sqlite3_bind_parameter_index(_insert_message_tag, "@TIMESTAMP"),
                    now.time_since_epoch().count()));
            ensure_success(
                ::sqlite3_bind_text(_insert_message_tag,
                    ::sqlite3_bind_parameter_index(_insert_message_tag, "@NAME"),
                    tag.name.c_str(),
                    static_cast<int>(tag.name.size()),
                    SQLITE_STATIC));
            ensure_success(
                ::sqlite3_bind_text(_insert_message_tag,
                    ::sqlite3_bind_parameter_index(_insert_message_tag, "@VALUE"),
                    tag.value.c_str(),
                    static_cast<int>(tag.value.size()),
                    SQLITE_STATIC));
            ensure_success(
                ::sqlite3_step(_insert_message_tag));
            ensure_success(
                ::sqlite3_reset(_insert_message_tag));
        }
        return now.time_since_epoch();
    };
};

sqlite::sqlite(const ::std::string& filename)
    : _chat{ new connection{} } {
    _chat->_filename = filename;
    _chat->on_init();
};

sqlite::iterator sqlite::insert(const message& message) {
    return _chat->on_insert(message);
};

} // histories
} // chat
} // ai

#endif
