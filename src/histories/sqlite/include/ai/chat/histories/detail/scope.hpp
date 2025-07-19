#ifndef AI_CHAT_HISTORIES_DETAIL_SCOPE_HPP
#define AI_CHAT_HISTORIES_DETAIL_SCOPE_HPP

#include <string>
#include <utility>
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

    scope() = delete;
    scope(scope const &other) = delete;
    scope(scope &&other) = delete;

    ~scope();

    scope & operator=(scope const &other) = delete;
    scope & operator=(scope &&other) = delete;

    explicit scope(::sqlite3 *database);

    void on_init();
    void on_advance();
    void on_commit();
    void on_rollback();
    void on_advance(ptrdiff_t dist);
    void on_advance(::std::chrono::nanoseconds dist);
    ptrdiff_t on_count(::std::chrono::nanoseconds last) const;

    ::sqlite3 *_database;
    ::sqlite3_stmt *_commit;
    ::sqlite3_stmt *_rollback;
    ::sqlite3_stmt *_s_message;
    ::sqlite3_stmt *_s_message_content;
    ::sqlite3_stmt *_s_message_tag;
    ::sqlite3_stmt *_s_tag_name;
    ::sqlite3_stmt *_s_tag_value;
    ::sqlite3_stmt *_s_count;
    ::sqlite3_stmt *_s_tag_name_id;
    ::sqlite3_stmt *_s_tag_value_id;
    ::std::string _statement;
    ::std::vector<::std::pair<::sqlite3_int64, ::sqlite3_int64>> _filters;
    ::std::chrono::nanoseconds _timestamp;
    ::std::string _content;
    ::std::vector<::std::string> _tag_names;
    ::std::vector<::std::string> _tag_values;
    ::std::vector<tag> _tags;
    int _exceptions;
};

} // detail
} // histories
} // chat
} // ai

#include "impl/scope.ipp"

#endif
