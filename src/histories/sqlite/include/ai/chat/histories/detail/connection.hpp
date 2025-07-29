#ifndef AI_CHAT_HISTORIES_DETAIL_CONNECTION_HPP
#define AI_CHAT_HISTORIES_DETAIL_CONNECTION_HPP

#include <string>

#include "sqlite3.h"

#include "ai/chat/histories/sqlite.hpp"
#include "ai/chat/telemetry.hpp"

namespace ai {
namespace chat {
namespace histories {
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
    void on_insert_begin(::sqlite3_stmt **commit, ::sqlite3_stmt **rollback
        DECLARE_SPAN(root));
    void on_insert_message(::std::chrono::nanoseconds timestamp, ::std::string_view content
        DECLARE_SPAN(root));
    void on_insert_message_tag(::std::chrono::nanoseconds timestamp, ::std::string_view tag_name, ::std::string_view tag_value
        DECLARE_SPAN(root));
    void on_erase_begin(::sqlite3_stmt **commit, ::sqlite3_stmt **rollback
        DECLARE_SPAN(root));
    void on_erase_message(::std::chrono::nanoseconds first, ::std::chrono::nanoseconds last
        DECLARE_SPAN(root));

    ::std::string _filename;
    ::sqlite3 *_database;
    ::sqlite3_stmt *_init_wal;
    ::sqlite3_stmt *_init_message_content;
    ::sqlite3_stmt *_init_tag_name;
    ::sqlite3_stmt *_init_tag_value;
    ::sqlite3_stmt *_init_message;
    ::sqlite3_stmt *_i_begin;
    ::sqlite3_stmt *_i_message_content;
    ::sqlite3_stmt *_i_tag_name;
    ::sqlite3_stmt *_i_tag_value;
    ::sqlite3_stmt *_i_message;
    ::sqlite3_stmt *_i_message_tag;
    ::sqlite3_stmt *_d_begin;
    ::sqlite3_stmt *_d_message;
    ::sqlite3_stmt *_d_message_content;
    ::sqlite3_stmt *_s_message_content;
    ::sqlite3_stmt *_s_message_tag;
    ::sqlite3_stmt *_s_tag_name;
    ::sqlite3_stmt *_s_tag_value;
    ::sqlite3_stmt *_s_count;
    ::sqlite3_stmt *_s_tag_name_id;
    ::sqlite3_stmt *_s_tag_value_id;
    ::sqlite3_stmt *_u_begin;
    ::sqlite3_stmt *_u_message_content;
    
    DELCARE_TRACER()
};

} // detail
} // histories
} // chat
} // ai

#include "impl/connection.ipp"

#endif
