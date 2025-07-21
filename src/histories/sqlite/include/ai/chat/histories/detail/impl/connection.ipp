#ifndef AI_CHAT_HISTORIES_DETAIL_CONNECTION_IPP
#define AI_CHAT_HISTORIES_DETAIL_CONNECTION_IPP

#include "esqlite3.hpp"

namespace ai {
namespace chat {
namespace histories {
namespace detail {

connection::connection()
    : _filename{}
    , _database{ nullptr }
    , _init_wal{ nullptr }
    , _init_message_content{ nullptr }
    , _init_tag_name{ nullptr }
    , _init_tag_value{ nullptr }
    , _init_message{ nullptr }
    , _i_begin{ nullptr }
    , _i_message_content{ nullptr }
    , _i_tag_name{ nullptr }
    , _i_tag_value{ nullptr }
    , _i_message{ nullptr }
    , _i_message_tag{ nullptr }
    , _d_begin{ nullptr }
    , _d_message{ nullptr }
    , _d_message_content{ nullptr }
    , _s_message_content{ nullptr }
    , _s_message_tag{ nullptr }
    , _s_tag_name{ nullptr }
    , _s_tag_value{ nullptr }
    , _s_count{ nullptr }
    , _s_tag_name_id{ nullptr }
    , _s_tag_value_id{ nullptr }
    , _u_begin{ nullptr }
    , _u_message_content{ nullptr }
    , _tracer{
        ::opentelemetry::trace::Provider::GetTracerProvider()
            ->GetTracer("ai_chat_histories_sqlite")
    } {

};

connection::~connection() {
    ::sqlite3_finalize(_u_message_content);
    ::sqlite3_finalize(_u_begin);
    ::sqlite3_finalize(_s_tag_value_id);
    ::sqlite3_finalize(_s_tag_name_id);
    ::sqlite3_finalize(_s_count);
    ::sqlite3_finalize(_s_tag_value);
    ::sqlite3_finalize(_s_tag_name);
    ::sqlite3_finalize(_s_message_tag);
    ::sqlite3_finalize(_s_message_content);
    ::sqlite3_finalize(_d_message_content);
    ::sqlite3_finalize(_d_message);
    ::sqlite3_finalize(_d_begin);
    ::sqlite3_finalize(_i_message_tag);
    ::sqlite3_finalize(_i_message);
    ::sqlite3_finalize(_i_tag_value);
    ::sqlite3_finalize(_i_tag_name);
    ::sqlite3_finalize(_i_message_content);
    ::sqlite3_finalize(_i_begin);
    ::sqlite3_finalize(_init_message);
    ::sqlite3_finalize(_init_tag_value);
    ::sqlite3_finalize(_init_tag_name);
    ::sqlite3_finalize(_init_message_content);
    ::sqlite3_finalize(_init_wal);
    ::sqlite3_close(_database);
};

void connection::on_init() {
    ::esqlite3_ensure_success(
        ::sqlite3_open_v2(_filename.c_str(), &_database,
            SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE,
            nullptr));
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
    char const INIT_MESSAGE_CONTENT[]{
        "CREATE TABLE IF NOT EXISTS message_content"
        "("
                "timestamp INTEGER NOT NULL CONSTRAINT PK_message PRIMARY KEY"
            ", content TEXT NOT NULL"
        ") WITHOUT ROWID"
    };
    ::esqlite3_ensure_success(
        ::sqlite3_prepare_v2(_database, INIT_MESSAGE_CONTENT,
            static_cast<int>(::std::size(INIT_MESSAGE_CONTENT) - 1),
            &_init_message_content, nullptr));
    ::esqlite3_ensure_success(
        ::sqlite3_step(_init_message_content));
    ::esqlite3_ensure_success(
        ::sqlite3_finalize(_init_message_content));
    _init_message_content = nullptr;
    char const INIT_TAG_NAME[]{
        "CREATE TABLE IF NOT EXISTS tag_name"
        "("
                "id INTEGER NOT NULL CONSTRAINT PK_tag_name PRIMARY KEY AUTOINCREMENT"
            ", name TEXT NOT NULL CONSTRAINT UQ_tag_name_name UNIQUE"
        ")"
    };
    ::esqlite3_ensure_success(
        ::sqlite3_prepare_v2(_database, INIT_TAG_NAME,
            static_cast<int>(::std::size(INIT_TAG_NAME) - 1),
            &_init_tag_name, nullptr));
    ::esqlite3_ensure_success(
        ::sqlite3_step(_init_tag_name));
    ::esqlite3_ensure_success(
        ::sqlite3_finalize(_init_tag_name));
    _init_tag_name = nullptr;
    char const INIT_TAG_VALUE[]{
        "CREATE TABLE IF NOT EXISTS tag_value"
        "("
                "id INTEGER NOT NULL CONSTRAINT PK_tag_value PRIMARY KEY AUTOINCREMENT"
            ", value TEXT NOT NULL CONSTRAINT UQ_tag_value_value UNIQUE"
        ")"
    };
    ::esqlite3_ensure_success(
        ::sqlite3_prepare_v2(_database, INIT_TAG_VALUE,
            static_cast<int>(::std::size(INIT_TAG_VALUE) - 1),
            &_init_tag_value, nullptr));
    ::esqlite3_ensure_success(
        ::sqlite3_step(_init_tag_value));
    ::esqlite3_ensure_success(
        ::sqlite3_finalize(_init_tag_value));
    _init_tag_value = nullptr;
    char const INIT_MESSAGE[]{
        "CREATE TABLE IF NOT EXISTS message"
        "("
                "timestamp INTEGER NOT NULL CONSTRAINT FK_message_message_content REFERENCES message_content(timestamp)"
            ", name_id INTEGER CONSTRAINT FK_message_tag_name REFERENCES tag_name(id)"
            ", value_id INTEGER CONSTRAINT FK_message_tag_value REFERENCES tag_value(id)"
            ", CONSTRAINT PK_message PRIMARY KEY(timestamp, name_id, value_id)"
        ")"
    };
    ::esqlite3_ensure_success(
        ::sqlite3_prepare_v2(_database, INIT_MESSAGE,
            static_cast<int>(::std::size(INIT_MESSAGE) - 1),
            &_init_message, nullptr));
    ::esqlite3_ensure_success(
        ::sqlite3_step(_init_message));
    ::esqlite3_ensure_success(
        ::sqlite3_finalize(_init_message));
    _init_message = nullptr;
    char const INSERT_BEGIN[]{
        "SAVEPOINT insert_message"
    };
    ::esqlite3_ensure_success(
        ::sqlite3_prepare_v2(_database, INSERT_BEGIN,
            static_cast<int>(::std::size(INSERT_BEGIN) - 1),
            &_i_begin, nullptr));
    char const INSERT_MESSAGE_CONTENT[]{
        "INSERT INTO message_content"
        "("
            "timestamp, content"
        ")"
        " VALUES"
        "("
            "@TIMESTAMP, @CONTENT"
        ")"
    };
    ::esqlite3_ensure_success(
        ::sqlite3_prepare_v2(_database, INSERT_MESSAGE_CONTENT,
            static_cast<int>(::std::size(INSERT_MESSAGE_CONTENT) - 1),
            &_i_message_content, nullptr));
    char const INSERT_TAG_NAME[]{
        "INSERT OR IGNORE INTO tag_name"
        "("
            "name"
        ")"
        " VALUES"
        "("
            "@NAME"
        ")"
    };
    ::esqlite3_ensure_success(
        ::sqlite3_prepare_v2(_database, INSERT_TAG_NAME,
            static_cast<int>(::std::size(INSERT_TAG_NAME) - 1),
            &_i_tag_name, nullptr));
    char const INSERT_TAG_VALUE[]{
        "INSERT OR IGNORE INTO tag_value"
        "("
            "value"
        ")"
        " VALUES"
        "("
            "@VALUE"
        ")"
    };
    ::esqlite3_ensure_success(
        ::sqlite3_prepare_v2(_database, INSERT_TAG_VALUE,
            static_cast<int>(::std::size(INSERT_TAG_VALUE) - 1),
            &_i_tag_value, nullptr));
    char const INSERT_MESSAGE[]{
        "INSERT INTO message"
        "("
            "timestamp"
        ")"
        " VALUES"
        "("
            "@TIMESTAMP"
        ")"
    };
    ::esqlite3_ensure_success(
        ::sqlite3_prepare_v2(_database, INSERT_MESSAGE,
            static_cast<int>(::std::size(INSERT_MESSAGE) - 1),
            &_i_message, nullptr));
    char const INSERT_MESSAGE_TAG[]{
        "INSERT INTO message"
        "("
            "timestamp, name_id, value_id"
        ")"
        " VALUES"
        "("
            "@TIMESTAMP, @TAG_NAME_ID, @TAG_VALUE_ID"
        ")"
    };
    ::esqlite3_ensure_success(
        ::sqlite3_prepare_v2(_database, INSERT_MESSAGE_TAG,
            static_cast<int>(::std::size(INSERT_MESSAGE_TAG) - 1),
            &_i_message_tag, nullptr));
    char const DELETE_BEGIN[]{
        "SAVEPOINT delete_message"
    };
    ::esqlite3_ensure_success(
        ::sqlite3_prepare_v2(_database, DELETE_BEGIN,
            static_cast<int>(::std::size(DELETE_BEGIN) - 1),
            &_d_begin, nullptr));
    char const DELETE_MESSAGE[]{
        "DELETE FROM message"
        " WHERE timestamp BETWEEN @FIRST AND @LAST - 1"
    };
    ::esqlite3_ensure_success(
        ::sqlite3_prepare_v2(_database, DELETE_MESSAGE,
            static_cast<int>(::std::size(DELETE_MESSAGE) - 1),
            &_d_message, nullptr));
    char const DELETE_MESSAGE_CONTENT[]{
        "DELETE FROM message_content"
        " WHERE timestamp BETWEEN @FIRST AND @LAST - 1"
    };
    ::esqlite3_ensure_success(
        ::sqlite3_prepare_v2(_database, DELETE_MESSAGE_CONTENT,
            static_cast<int>(::std::size(DELETE_MESSAGE_CONTENT) - 1),
            &_d_message_content, nullptr));
    char const SELECT_MESSAGE_CONTENT[]{
        "SELECT content FROM message_content"
        " WHERE timestamp = @TIMESTAMP"
    };
    ::esqlite3_ensure_success(
        ::sqlite3_prepare_v2(_database, SELECT_MESSAGE_CONTENT,
            static_cast<int>(::std::size(SELECT_MESSAGE_CONTENT) - 1),
            &_s_message_content, nullptr));
    char const SELECT_MESSAGE_TAG[]{
        "SELECT name_id, value_id FROM message"
        " WHERE timestamp = @TIMESTAMP"
        " AND name_id IS NOT NULL"
        " AND value_id IS NOT NULL"
    };
    ::esqlite3_ensure_success(
        ::sqlite3_prepare_v2(_database, SELECT_MESSAGE_TAG,
            static_cast<int>(::std::size(SELECT_MESSAGE_TAG) - 1),
            &_s_message_tag, nullptr));
    char const SELECT_TAG_NAME[]{
        "SELECT name FROM tag_name"
        " WHERE id = @ID"
    };
    ::esqlite3_ensure_success(
        ::sqlite3_prepare_v2(_database, SELECT_TAG_NAME,
            static_cast<int>(::std::size(SELECT_TAG_NAME) - 1),
            &_s_tag_name, nullptr));
    char const SELECT_TAG_VALUE[]{
        "SELECT value FROM tag_value"
        " WHERE id = @ID"
    };
    ::esqlite3_ensure_success(
        ::sqlite3_prepare_v2(_database, SELECT_TAG_VALUE,
            static_cast<int>(::std::size(SELECT_TAG_VALUE) - 1),
            &_s_tag_value, nullptr));
    char const SELECT_COUNT[]{
        "SELECT COUNT(*) FROM message_content"
        " WHERE timestamp BETWEEN @FIRST AND @LAST - 1"
    };
    ::esqlite3_ensure_success(
        ::sqlite3_prepare_v2(_database, SELECT_COUNT,
            static_cast<int>(::std::size(SELECT_COUNT) - 1),
            &_s_count, nullptr));
    char const SELECT_TAG_NAME_ID[]{
        "SELECT id FROM tag_name"
        " WHERE name = @NAME"
    };
    ::esqlite3_ensure_success(
        ::sqlite3_prepare_v2(_database, SELECT_TAG_NAME_ID,
            static_cast<int>(::std::size(SELECT_TAG_NAME_ID) - 1),
            &_s_tag_name_id, nullptr));
    char const SELECT_TAG_VALUE_ID[]{
        "SELECT id FROM tag_value"
        " WHERE value = @VALUE"
    };
    ::esqlite3_ensure_success(
        ::sqlite3_prepare_v2(_database, SELECT_TAG_VALUE_ID,
            static_cast<int>(::std::size(SELECT_TAG_VALUE_ID) - 1),
            &_s_tag_value_id, nullptr));
    char const UPDATE_BEGIN[]{
        "SAVEPOINT update_message"
    };
    ::esqlite3_ensure_success(
        ::sqlite3_prepare_v2(_database, UPDATE_BEGIN,
            static_cast<int>(::std::size(UPDATE_BEGIN) - 1),
            &_u_begin, nullptr));
    char const UPDATE_MESSAGE_CONTENT[]{
        "UPDATE message_content"
        " SET content = @CONTENT"
        " WHERE timestamp = @TIMESTAMP"
    };
    ::esqlite3_ensure_success(
        ::sqlite3_prepare_v2(_database, UPDATE_MESSAGE_CONTENT,
            static_cast<int>(::std::size(UPDATE_MESSAGE_CONTENT) - 1),
            &_u_message_content, nullptr));
};
void connection::on_insert_begin(::sqlite3_stmt **commit, ::sqlite3_stmt **rollback,
    ::opentelemetry::nostd::shared_ptr<::opentelemetry::trace::Span> root) {
    ::opentelemetry::nostd::shared_ptr<::opentelemetry::trace::Span> span{
        _tracer->StartSpan("on_insert_begin", ::opentelemetry::trace::StartSpanOptions{
            {}, {},
            root->GetContext()
        })
    };
    ::esqlite3_ensure_success(
        ::sqlite3_step(_i_begin));
    ::esqlite3_ensure_success(
        ::sqlite3_reset(_i_begin));
    char const INSERT_COMMIT[]{
        "RELEASE SAVEPOINT insert_message"
    };
    ::esqlite3_ensure_success(
        ::sqlite3_prepare_v2(_database, INSERT_COMMIT,
            static_cast<int>(::std::size(INSERT_COMMIT) - 1),
            commit, nullptr));
    char const INSERT_ROLLBACK[]{
        "ROLLBACK TO SAVEPOINT insert_message"
    };
    ::esqlite3_ensure_success(
        ::sqlite3_prepare_v2(_database, INSERT_ROLLBACK,
            static_cast<int>(::std::size(INSERT_ROLLBACK) - 1),
            rollback, nullptr));
};
void connection::on_insert_message(::std::chrono::nanoseconds timestamp, ::std::string_view content,
    ::opentelemetry::nostd::shared_ptr<::opentelemetry::trace::Span> root) {
    ::opentelemetry::nostd::shared_ptr<::opentelemetry::trace::Span> span{
        _tracer->StartSpan("on_insert_message", ::opentelemetry::trace::StartSpanOptions{
            {}, {},
            root->GetContext()
        })
    };
    ::esqlite3_ensure_success(
        ::sqlite3_bind_int64(_i_message_content,
            ::sqlite3_bind_parameter_index(_i_message_content, "@TIMESTAMP"),
            timestamp.count()));
    ::esqlite3_ensure_success(
        ::sqlite3_bind_text(_i_message_content,
            ::sqlite3_bind_parameter_index(_i_message_content, "@CONTENT"),
            content.data(),
            static_cast<int>(content.size()),
            SQLITE_STATIC));
    ::esqlite3_ensure_success(
        ::sqlite3_step(_i_message_content));
    ::esqlite3_ensure_success(
        ::sqlite3_reset(_i_message_content));
    ::esqlite3_ensure_success(
        ::sqlite3_bind_int64(_i_message,
            ::sqlite3_bind_parameter_index(_i_message, "@TIMESTAMP"),
            timestamp.count()));
    ::esqlite3_ensure_success(
        ::sqlite3_step(_i_message));
    ::esqlite3_ensure_success(
        ::sqlite3_reset(_i_message));
};
void connection::on_insert_message_tag(::std::chrono::nanoseconds timestamp, ::std::string_view tag_name, ::std::string_view tag_value,
    ::opentelemetry::nostd::shared_ptr<::opentelemetry::trace::Span> root) {
    ::opentelemetry::nostd::shared_ptr<::opentelemetry::trace::Span> span{
        _tracer->StartSpan("on_insert_message_tag", ::opentelemetry::trace::StartSpanOptions{
            {}, {},
            root->GetContext()
        })
    };
    ::esqlite3_ensure_success(
        ::sqlite3_bind_text(_i_tag_name,
            ::sqlite3_bind_parameter_index(_i_tag_name, "@NAME"),
            tag_name.data(),
            static_cast<int>(tag_name.size()),
            SQLITE_STATIC));
    ::esqlite3_ensure_success(
        ::sqlite3_step(_i_tag_name));
    ::esqlite3_ensure_success(
        ::sqlite3_reset(_i_tag_name));
    ::esqlite3_ensure_success(
        ::sqlite3_bind_text(_i_tag_value,
            ::sqlite3_bind_parameter_index(_i_tag_value, "@VALUE"),
            tag_value.data(),
            static_cast<int>(tag_value.size()),
            SQLITE_STATIC));
    ::esqlite3_ensure_success(
        ::sqlite3_step(_i_tag_value));
    ::esqlite3_ensure_success(
        ::sqlite3_reset(_i_tag_value));
    ::esqlite3_ensure_success(
        ::sqlite3_bind_text(_s_tag_name_id,
            ::sqlite3_bind_parameter_index(_s_tag_name_id, "@NAME"),
            tag_name.data(),
            static_cast<int>(tag_name.size()),
            SQLITE_STATIC));
    ::esqlite3_ensure_success(
        ::sqlite3_step(_s_tag_name_id));
    ::sqlite3_int64 tag_name_id{ ::sqlite3_column_int64(_s_tag_name_id, 0) };
    ::esqlite3_ensure_success(
        ::sqlite3_reset(_s_tag_name_id));
    ::esqlite3_ensure_success(
        ::sqlite3_bind_text(_s_tag_value_id,
            ::sqlite3_bind_parameter_index(_s_tag_value_id, "@VALUE"),
            tag_value.data(),
            static_cast<int>(tag_value.size()),
            SQLITE_STATIC));
    ::esqlite3_ensure_success(
        ::sqlite3_step(_s_tag_value_id));
    ::sqlite3_int64 tag_value_id{ ::sqlite3_column_int64(_s_tag_value_id, 0) };
    ::esqlite3_ensure_success(
        ::sqlite3_reset(_s_tag_value_id));
    ::esqlite3_ensure_success(
        ::sqlite3_bind_int64(_i_message_tag,
            ::sqlite3_bind_parameter_index(_i_message_tag, "@TIMESTAMP"),
            timestamp.count()));
    ::esqlite3_ensure_success(
        ::sqlite3_bind_int64(_i_message_tag,
            ::sqlite3_bind_parameter_index(_i_message_tag, "@TAG_NAME_ID"),
            tag_name_id));
    ::esqlite3_ensure_success(
        ::sqlite3_bind_int64(_i_message_tag,
            ::sqlite3_bind_parameter_index(_i_message_tag, "@TAG_VALUE_ID"),
            tag_value_id));
    ::esqlite3_ensure_success(
        ::sqlite3_step(_i_message_tag));
    ::esqlite3_ensure_success(
        ::sqlite3_reset(_i_message_tag));
};
void connection::on_erase_begin(::sqlite3_stmt **commit, ::sqlite3_stmt **rollback,
    ::opentelemetry::nostd::shared_ptr<::opentelemetry::trace::Span> root) {
    ::opentelemetry::nostd::shared_ptr<::opentelemetry::trace::Span> span{
        _tracer->StartSpan("on_erase_begin", ::opentelemetry::trace::StartSpanOptions{
            {}, {},
            root->GetContext()
        })
    };
    ::esqlite3_ensure_success(
        ::sqlite3_step(_d_begin));
    ::esqlite3_ensure_success(
        ::sqlite3_reset(_d_begin));
    ::std::pair<::sqlite3_stmt *, ::sqlite3_stmt *> commit_n_rollback{};
    char const INSERT_COMMIT[]{
        "RELEASE SAVEPOINT delete_message"
    };
    ::esqlite3_ensure_success(
        ::sqlite3_prepare_v2(_database, INSERT_COMMIT,
            static_cast<int>(::std::size(INSERT_COMMIT) - 1),
            commit, nullptr));
    char const INSERT_ROLLBACK[]{
        "ROLLBACK TO SAVEPOINT delete_message"
    };
    ::esqlite3_ensure_success(
        ::sqlite3_prepare_v2(_database, INSERT_ROLLBACK,
            static_cast<int>(::std::size(INSERT_ROLLBACK) - 1),
            rollback, nullptr));
};
void connection::on_erase_message(::std::chrono::nanoseconds first, ::std::chrono::nanoseconds last,
    ::opentelemetry::nostd::shared_ptr<::opentelemetry::trace::Span> root) {
    ::opentelemetry::nostd::shared_ptr<::opentelemetry::trace::Span> span{
        _tracer->StartSpan("on_erase_message", ::opentelemetry::trace::StartSpanOptions{
            {}, {},
            root->GetContext()
        })
    };
    ::esqlite3_ensure_success(
        ::sqlite3_bind_int64(_d_message,
            ::sqlite3_bind_parameter_index(_d_message, "@FIRST"),
            first.count()));
    ::esqlite3_ensure_success(
        ::sqlite3_bind_int64(_d_message,
            ::sqlite3_bind_parameter_index(_d_message, "@LAST"),
            last.count()));
    ::esqlite3_ensure_success(
        ::sqlite3_step(_d_message));
    ::esqlite3_ensure_success(
        ::sqlite3_reset(_d_message));
    ::esqlite3_ensure_success(
        ::sqlite3_bind_int64(_d_message_content,
            ::sqlite3_bind_parameter_index(_d_message_content, "@FIRST"),
            first.count()));
    ::esqlite3_ensure_success(
        ::sqlite3_bind_int64(_d_message_content,
            ::sqlite3_bind_parameter_index(_d_message_content, "@LAST"),
            last.count()));
    ::esqlite3_ensure_success(
        ::sqlite3_step(_d_message_content));
    ::esqlite3_ensure_success(
        ::sqlite3_reset(_d_message_content));
};

} // detail
} // histories
} // chat
} // ai

#endif
