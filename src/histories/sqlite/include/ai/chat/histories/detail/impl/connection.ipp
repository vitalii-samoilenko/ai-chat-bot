#ifndef AI_CHAT_HISTORIES_DETAIL_CONNECTION_IPP
#define AI_CHAT_HISTORIES_DETAIL_CONNECTION_IPP

#include "esqlite3/ensure_success.hpp"

#include "ai/chat/histories/detail/connection.hpp"

namespace ai {
namespace chat {
namespace histories {
namespace detail {

connection::connection()
    : _filename{}
    , _database{ nullptr }
    , _init_wal{ nullptr }
    , _init_message{ nullptr }
    , _init_tag_name{ nullptr }
    , _init_tag_value{ nullptr }
    , _init_message_tag{ nullptr }
    , _i_begin{ nullptr }
    , _i_message{ nullptr }
    , _i_tag_name{ nullptr }
    , _i_tag_value{ nullptr }
    , _i_message_tag{ nullptr }
    , _d_begin{ nullptr }
    , _d_message_tag{ nullptr }
    , _d_message{ nullptr }
    , _s_message_tag{ nullptr }
    , _s_tag_name{ nullptr }
    , _s_tag_value{ nullptr }
    , _s_count{ nullptr }
    , _tracer{
        ::opentelemetry::trace::Provider::GetTracerProvider()
            ->GetTracer("ai_chat_histories_sqlite")
    } {

};

connection::~connection() {
    ::sqlite3_finalize(_s_count);
    ::sqlite3_finalize(_s_tag_value);
    ::sqlite3_finalize(_s_tag_name);
    ::sqlite3_finalize(_s_message_tag);
    ::sqlite3_finalize(_d_message);
    ::sqlite3_finalize(_d_message_tag);
    ::sqlite3_finalize(_d_begin);
    ::sqlite3_finalize(_i_message_tag);
    ::sqlite3_finalize(_i_tag_value);
    ::sqlite3_finalize(_i_tag_name);
    ::sqlite3_finalize(_i_message);
    ::sqlite3_finalize(_i_begin);
    ::sqlite3_finalize(_init_message_tag);
    ::sqlite3_finalize(_init_tag_value);
    ::sqlite3_finalize(_init_tag_name);
    ::sqlite3_finalize(_init_message);
    ::sqlite3_finalize(_init_wal);
    ::sqlite3_close(_database);
};

void connection::on_init() {
    ::esqlite3_ensure_success(
        ::sqlite3_open_v2(_filename.c_str(), &_database,
            SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE,
            nullptr));
    const char INIT_WAL[]{
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
    const char INIT_MESSAGE[]{
        "CREATE TABLE IF NOT EXISTS message"
        "("
                "timestamp INTEGER NOT NULL CONSTRAINT PK_message PRIMARY KEY"
            ", content TEXT NOT NULL"
        ") WITHOUT ROWID"
    };
    ::esqlite3_ensure_success(
        ::sqlite3_prepare_v2(_database, INIT_MESSAGE,
            static_cast<int>(::std::size(INIT_MESSAGE) - 1),
            &_init_message, nullptr));
    ::esqlite3_ensure_success(
        ::sqlite3_step(_init_message));
    ::esqlite3_ensure_success(
        ::sqlite3_finalize(_init_message));
    const char INIT_TAG_NAME[]{
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
    const char INIT_TAG_VALUE[]{
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
    const char INIT_MESSAGE_TAG[]{
        "CREATE TABLE IF NOT EXISTS message_tag"
        "("
                "timestamp INTEGER NOT NULL CONSTRAINT FK_message_tag_message REFERENCES message(timestamp)"
            ", name_id INTEGER NOT NULL CONSTRAINT FK_message_tag_tag_name REFERENCES tag_name(id)"
            ", value_id INTEGER NOT NULL CONSTRAINT FK_message_tag_tag_value REFERENCES tag_value(id)"
            ", CONSTRAINT PK_message_tag PRIMARY KEY(timestamp, name_id, value_id)"
        ") WITHOUT ROWID"
    };
    ::esqlite3_ensure_success(
        ::sqlite3_prepare_v2(_database, INIT_MESSAGE_TAG,
            static_cast<int>(::std::size(INIT_MESSAGE_TAG) - 1),
            &_init_message_tag, nullptr));
    ::esqlite3_ensure_success(
        ::sqlite3_step(_init_message_tag));
    ::esqlite3_ensure_success(
        ::sqlite3_finalize(_init_message_tag));
    const char INSERT_BEGIN[]{
        "SAVEPOINT insert_message"
    };
    ::esqlite3_ensure_success(
        ::sqlite3_prepare_v2(_database, INSERT_BEGIN,
            static_cast<int>(::std::size(INSERT_BEGIN) - 1),
            &_i_begin, nullptr));
    const char INSERT_MESSAGE[]{
        "INSERT INTO message"
        "("
            "timestamp, content"
        ")"
        " VALUES"
        "("
            "@TIMESTAMP, @CONTENT"
        ")"
    };
    ::esqlite3_ensure_success(
        ::sqlite3_prepare_v2(_database, INSERT_MESSAGE,
            static_cast<int>(::std::size(INSERT_MESSAGE) - 1),
            &_i_message, nullptr));
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
    ::esqlite3_ensure_success(
        ::sqlite3_prepare_v2(_database, INSERT_TAG_NAME,
            static_cast<int>(::std::size(INSERT_TAG_NAME) - 1),
            &_i_tag_name, nullptr));
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
    ::esqlite3_ensure_success(
        ::sqlite3_prepare_v2(_database, INSERT_TAG_VALUE,
            static_cast<int>(::std::size(INSERT_TAG_VALUE) - 1),
            &_i_tag_value, nullptr));
    const char INSERT_MESSAGE_TAG[]{
        "INSERT OR IGNORE INTO message_tag"
        "("
            "timestamp, name_id, value_id"
        ")"
        " SELECT @TIMESTAMP"
        ", (SELECT id FROM tag_name WHERE name = @NAME)"
        ", (SELECT id FROM tag_value WHERE value = @VALUE)"
    };
    ::esqlite3_ensure_success(
        ::sqlite3_prepare_v2(_database, INSERT_MESSAGE_TAG,
            static_cast<int>(::std::size(INSERT_MESSAGE_TAG) - 1),
            &_i_message_tag, nullptr));
    const char DELETE_BEGIN[]{
        "SAVEPOINT delete_message"
    };
    ::esqlite3_ensure_success(
        ::sqlite3_prepare_v2(_database, DELETE_BEGIN,
            static_cast<int>(::std::size(DELETE_BEGIN) - 1),
            &_d_begin, nullptr));
    const char DELETE_MESSAGE_TAG[]{
        "DELETE FROM message_tag"
        " WHERE timestamp IS BETWEEN @FIRST AND @LAST - 1"
    };
    ::esqlite3_ensure_success(
        ::sqlite3_prepare_v2(_database, DELETE_MESSAGE_TAG,
            static_cast<int>(::std::size(DELETE_MESSAGE_TAG) - 1),
            &_d_message_tag, nullptr));
    const char DELETE_MESSAGE[]{
        "DELETE FROM message"
        " WHERE timestamp IS BETWEEN @FIRST AND @LAST - 1"
    };
    ::esqlite3_ensure_success(
        ::sqlite3_prepare_v2(_database, DELETE_MESSAGE,
            static_cast<int>(::std::size(DELETE_MESSAGE) - 1),
            &_d_message, nullptr));
    const char SELECT_MESSAGE_TAG[]{
        "SELECT name_id, value_id FROM message_tag"
        " WHERE timestamp = @TIMESTAMP"
    };
    ::esqlite3_ensure_success(
        ::sqlite3_prepare_v2(_database, SELECT_MESSAGE_TAG,
            static_cast<int>(::std::size(SELECT_MESSAGE_TAG) - 1),
            &_s_message_tag, nullptr));
    const char SELECT_TAG_NAME[]{
        "SELECT name FROM tag_name"
        " WHERE id = @ID"
    };
    ::esqlite3_ensure_success(
        ::sqlite3_prepare_v2(_database, SELECT_TAG_NAME,
            static_cast<int>(::std::size(SELECT_TAG_NAME) - 1),
            &_s_tag_name, nullptr));
    const char SELECT_TAG_VALUE[]{
        "SELECT value FROM tag_value"
        " WHERE id = @ID"
    };
    ::esqlite3_ensure_success(
        ::sqlite3_prepare_v2(_database, SELECT_TAG_VALUE,
            static_cast<int>(::std::size(SELECT_TAG_VALUE) - 1),
            &_s_tag_value, nullptr));
    const char SELECT_COUNT[]{
        "SELECT COUNT(*) FROM message"
        " WHERE timestamp BETWEEN @FIRST AND @LAST - 1"
    };
    ::esqlite3_ensure_success(
        ::sqlite3_prepare_v2(_database, SELECT_COUNT,
            static_cast<int>(::std::size(SELECT_COUNT) - 1),
            &_s_count, nullptr));
};
::std::pair<::sqlite3_stmt *, ::sqlite3_stmt *> connection::on_insert_begin(
    ::opentelemetry::nostd::shared_ptr<::opentelemetry::trace::Span> root) {
    ::opentelemetry::nostd::shared_ptr<::opentelemetry::trace::Span> span{
        _tracer->StartSpan("on_insert_begin", ::opentelemetry::trace::StartSpanOptions{
            {}, {},
            root->GetContext()
        })
    };
    ::esqlite3_ensure_success(
        ::sqlite3_reset(_i_begin));
    ::std::pair<::sqlite3_stmt *, ::sqlite3_stmt *> commit_n_rollback{};
    const char INSERT_COMMIT[]{
        "RELEASE SAVEPOINT insert_message"
    };
    ::esqlite3_ensure_success(
        ::sqlite3_prepare_v2(_database, INSERT_COMMIT,
            static_cast<int>(::std::size(INSERT_COMMIT) - 1),
            &commit_n_rollback.first, nullptr));
    const char INSERT_ROLLBACK[]{
        "ROLLBACK TO SAVEPOINT insert_message"
    };
    ::esqlite3_ensure_success(
        ::sqlite3_prepare_v2(_database, INSERT_ROLLBACK,
            static_cast<int>(::std::size(INSERT_ROLLBACK) - 1),
            &commit_n_rollback.second, nullptr));
    ::esqlite3_ensure_success(
        ::sqlite3_step(_i_begin));
    return commit_n_rollback;
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
        ::sqlite3_bind_int64(_i_message,
            ::sqlite3_bind_parameter_index(_i_message, "@TIMESTAMP"),
            timestamp.count()));
    ::esqlite3_ensure_success(
        ::sqlite3_bind_text(_i_message,
            ::sqlite3_bind_parameter_index(_i_message, "@CONTENT"),
            content.data(),
            static_cast<int>(content.size()),
            SQLITE_STATIC));
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
        ::sqlite3_bind_int64(_i_message_tag,
            ::sqlite3_bind_parameter_index(_i_message_tag, "@TIMESTAMP"),
            timestamp.count()));
    ::esqlite3_ensure_success(
        ::sqlite3_bind_text(_i_message_tag,
            ::sqlite3_bind_parameter_index(_i_message_tag, "@NAME"),
            tag_name.data(),
            static_cast<int>(tag_name.size()),
            SQLITE_STATIC));
    ::esqlite3_ensure_success(
        ::sqlite3_bind_text(_i_message_tag,
            ::sqlite3_bind_parameter_index(_i_message_tag, "@VALUE"),
            tag_value.data(),
            static_cast<int>(tag_value.size()),
            SQLITE_STATIC));
    ::esqlite3_ensure_success(
        ::sqlite3_step(_i_message_tag));
    ::esqlite3_ensure_success(
        ::sqlite3_reset(_i_message_tag));
};
::std::pair<::sqlite3_stmt *, ::sqlite3_stmt *> connection::on_erase_begin(
    ::opentelemetry::nostd::shared_ptr<::opentelemetry::trace::Span> root) {
    ::opentelemetry::nostd::shared_ptr<::opentelemetry::trace::Span> span{
        _tracer->StartSpan("on_erase_begin", ::opentelemetry::trace::StartSpanOptions{
            {}, {},
            root->GetContext()
        })
    };
    ::esqlite3_ensure_success(
        ::sqlite3_reset(_d_begin));
    ::std::pair<::sqlite3_stmt *, ::sqlite3_stmt *> commit_n_rollback{};
    const char INSERT_COMMIT[]{
        "RELEASE SAVEPOINT delete_message"
    };
    ::esqlite3_ensure_success(
        ::sqlite3_prepare_v2(_database, INSERT_COMMIT,
            static_cast<int>(::std::size(INSERT_COMMIT) - 1),
            &commit_n_rollback.first, nullptr));
    const char INSERT_ROLLBACK[]{
        "ROLLBACK TO SAVEPOINT delete_message"
    };
    ::esqlite3_ensure_success(
        ::sqlite3_prepare_v2(_database, INSERT_ROLLBACK,
            static_cast<int>(::std::size(INSERT_ROLLBACK) - 1),
            &commit_n_rollback.second, nullptr));
    ::esqlite3_ensure_success(
        ::sqlite3_step(_d_begin));
    return commit_n_rollback;
};
void connection::on_erase_message_tag(::std::chrono::nanoseconds first, ::std::chrono::nanoseconds last,
    ::opentelemetry::nostd::shared_ptr<::opentelemetry::trace::Span> root) {
    ::opentelemetry::nostd::shared_ptr<::opentelemetry::trace::Span> span{
        _tracer->StartSpan("on_erase_message_tag", ::opentelemetry::trace::StartSpanOptions{
            {}, {},
            root->GetContext()
        })
    };
    ::esqlite3_ensure_success(
        ::sqlite3_bind_int64(_d_message_tag,
            ::sqlite3_bind_parameter_index(_d_message_tag, "@FIRST"),
            first.count()));
    ::esqlite3_ensure_success(
        ::sqlite3_bind_int64(_d_message_tag,
            ::sqlite3_bind_parameter_index(_d_message_tag, "@LAST"),
            last.count()));
    ::esqlite3_ensure_success(
        ::sqlite3_step(_d_message_tag));
    ::esqlite3_ensure_success(
        ::sqlite3_reset(_d_message_tag));
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
};

} // detail
} // histories
} // chat
} // ai

#endif
