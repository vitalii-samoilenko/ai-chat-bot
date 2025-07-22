#ifndef AI_CHAT_HISTORIES_DETAIL_SCOPE_HPP
#define AI_CHAT_HISTORIES_DETAIL_SCOPE_HPP

#include <string>
#include <vector>

#include "sqlite3.h"

namespace ai {
namespace chat {
namespace histories {
namespace detail {

class scope {
private:
    friend iterator;
    friend sqlite;

    enum class state {
        create,
        init,
        cursor,
        data
    };

    scope() = delete;
    scope(scope const &other) = delete;
    scope(scope &&other) = delete;

    ~scope();

    scope & operator=(scope const &other) = delete;
    scope & operator=(scope &&other) = delete;

    explicit scope(::sqlite3 *database);

    void on_commit();
    void on_rollback();
    void on_upgrade(state to) const;
    void on_advance();
    void on_advance(ptrdiff_t dist);
    void on_advance(::std::chrono::nanoseconds dist);
    ptrdiff_t on_count(::std::chrono::nanoseconds first) const;
    void on_filter(tag tag);
    void on_update_begin();
    void on_update(::std::string_view content);

    ::sqlite3 *_database;
    ::sqlite3_stmt *_commit;
    ::sqlite3_stmt *_rollback;
    ::sqlite3_stmt mutable *_s_message;
    ::sqlite3_stmt *_s_message_content;
    ::sqlite3_stmt *_s_message_tag;
    ::sqlite3_stmt *_s_tag_name;
    ::sqlite3_stmt *_s_tag_value;
    ::sqlite3_stmt *_s_count;
    ::sqlite3_stmt *_s_tag_name_id;
    ::sqlite3_stmt *_s_tag_value_id;
    ::sqlite3_stmt *_u_begin;
    ::sqlite3_stmt *_u_message_content;
    state mutable _state;
    int _exceptions;
    ::sqlite3_int64 mutable _offset;
    ::std::vector<::sqlite3_int64> _name_ids;
    ::std::vector<::sqlite3_int64> _value_ids;
    ::std::chrono::nanoseconds mutable _timestamp;
    ::std::string mutable _content;
    ::std::vector<::std::string> mutable _tag_names;
    ::std::vector<::std::string> mutable _tag_values;
    ::std::vector<tag> mutable _tags;
};

} // detail
} // histories
} // chat
} // ai

#include "impl/scope.ipp"

#endif
