#ifndef AI_CHAT_HISTORIES_DETAIL_CONNECTION_HPP
#define AI_CHAT_HISTORIES_DETAIL_CONNECTION_HPP

#include <string>
#include <utility>

#include "opentelemetry/trace/provider.h"
#include "sqlite3.h"

namespace ai {
namespace chat {
namespace histories {

class iterator;
class sqlite;

namespace detail {

class connection {
private:
    friend iterator;
    friend sqlite;

    connection();
    connection(connection const &other) = delete;
    connection(connection &&other) = delete;

    ~connection();

    connection & operator=(connection const &other) = delete;
    connection & operator=(connection &&other) = delete;

    void on_init();
    ::std::pair<::sqlite3_stmt *, ::sqlite3_stmt *> on_insert_begin(
        ::opentelemetry::nostd::shared_ptr<::opentelemetry::trace::Span> root);
    void on_insert_message(::std::chrono::nanoseconds timestamp, ::std::string_view content,
        ::opentelemetry::nostd::shared_ptr<::opentelemetry::trace::Span> root);
    void on_insert_message_tag(::std::chrono::nanoseconds timestamp, ::std::string_view tag_name, ::std::string_view tag_value,
        ::opentelemetry::nostd::shared_ptr<::opentelemetry::trace::Span> root);
    ::std::pair<::sqlite3_stmt *, ::sqlite3_stmt *> on_erase_begin(
        ::opentelemetry::nostd::shared_ptr<::opentelemetry::trace::Span> root);
    void on_erase_message_tag(::std::chrono::nanoseconds first, ::std::chrono::nanoseconds last,
        ::opentelemetry::nostd::shared_ptr<::opentelemetry::trace::Span> root);
    void on_erase_message(::std::chrono::nanoseconds first, ::std::chrono::nanoseconds last,
        ::opentelemetry::nostd::shared_ptr<::opentelemetry::trace::Span> root);

    ::std::string _filename;
    ::sqlite3 *_database;
    ::sqlite3_stmt *_init_wal;
    ::sqlite3_stmt *_init_message;
    ::sqlite3_stmt *_init_tag_name;
    ::sqlite3_stmt *_init_tag_value;
    ::sqlite3_stmt *_init_message_tag;
    ::sqlite3_stmt *_i_begin;
    ::sqlite3_stmt *_i_message;
    ::sqlite3_stmt *_i_tag_name;
    ::sqlite3_stmt *_i_tag_value;
    ::sqlite3_stmt *_i_message_tag;
    ::sqlite3_stmt *_d_begin;
    ::sqlite3_stmt *_d_message_tag;
    ::sqlite3_stmt *_d_message;
    ::sqlite3_stmt *_s_message_tag;
    ::sqlite3_stmt *_s_tag_name;
    ::sqlite3_stmt *_s_tag_value;
    ::sqlite3_stmt *_s_count;
    ::opentelemetry::nostd::shared_ptr<::opentelemetry::trace::Tracer> _tracer;
};

} // detail
} // histories
} // chat
} // ai

#include "impl/connection.ipp"

#endif
