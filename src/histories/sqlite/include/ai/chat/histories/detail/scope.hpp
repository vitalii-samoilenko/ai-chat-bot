#ifndef AI_CHAT_HISTORIES_DETAIL_SCOPE_HPP
#define AI_CHAT_HISTORIES_DETAIL_SCOPE_HPP

#include <string>
#include <vector>

#include "sqlite3.h"

namespace ai {
namespace chat {
namespace histories {

class iterator;
class sqlite;

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
    ::std::chrono::nanoseconds _timestamp;
    ::sqlite3_stmt *_commit;
    ::sqlite3_stmt *_rollback;
    int _exceptions;
    ::sqlite3_stmt *_s_message;
    ::sqlite3_stmt *_s_message_tag;
    ::sqlite3_stmt *_s_tag_name;
    ::sqlite3_stmt *_s_tag_value;
    ::sqlite3_stmt *_s_count;
    ::std::vector<::std::string> _tag_names;
    ::std::vector<::std::string> _tag_values;
    ::std::string_view _content;
    ::std::vector<tag> _tags;
};

} // detail
} // histories
} // chat
} // ai

#include "impl/scope.ipp"

#endif
