#ifndef AI_CHAT_MODERATORS_DETAIL_CONNECTION_HPP
#define AI_CHAT_MODERATORS_DETAIL_CONNECTION_HPP

#include <string>

#include "opentelemetry/trace/provider.h"
#include "sqlite3.h"

namespace ai {
namespace chat {
namespace moderators {
namespace detail {

class connection {
private:
    friend iterator;
    friend sqlite;

    enum role : ::sqlite3_int64 {
        none            = 0b000000000000000000000000000000000000000000000000000000000000,
        administrator   = 0b000000000000000000000000000000000000000000000000000000000001,
        moderator       = 0b000000000000000000000000000000000000000000000000000000000010,
        interlocutor    = 0b000000000000000000000000000000000000000000000000000000000100
    };

    connection();
    connection(connection const &other) = delete;
    connection(connection &&other) = delete;

    ~connection();

    connection & operator=(connection const &other) = delete;
    connection & operator=(connection &&other) = delete;

    void on_init();
    iterator on_is_allowed(::std::string_view username1, ::sqlite3_int64 role, ::sqlite3_int64 since,
        ::opentelemetry::nostd::shared_ptr<::opentelemetry::trace::Span> root);
    iterator on_is_allowed(::std::string_view username1, ::std::string_view username2, role role, ::sqlite3_int64 since,
        ::opentelemetry::nostd::shared_ptr<::opentelemetry::trace::Span> root);
    iterator on_is_filtered(::std::string_view content,
        ::opentelemetry::nostd::shared_ptr<::opentelemetry::trace::Span> root);
    iterator on_promote(::std::string_view username, role role,
        ::opentelemetry::nostd::shared_ptr<::opentelemetry::trace::Span> root);
    iterator on_demote(::std::string_view username, role role,
        ::opentelemetry::nostd::shared_ptr<::opentelemetry::trace::Span> root);
    iterator on_timeout(::std::string_view username, ::sqlite3_int64 since,
        ::opentelemetry::nostd::shared_ptr<::opentelemetry::trace::Span> root);
    iterator on_filter(::std::string_view name, ::std::string_view pattern,
        ::opentelemetry::nostd::shared_ptr<::opentelemetry::trace::Span> root);
    iterator on_discard(::std::string_view name,
        ::opentelemetry::nostd::shared_ptr<::opentelemetry::trace::Span> root);

    ::std::string _filename;
    ::sqlite3_int64 _length;
    ::sqlite3 *_database;
    ::sqlite3_stmt *_init_wal;
    ::sqlite3_stmt *_init_user;
    ::sqlite3_stmt *_init_filter;
    ::sqlite3_stmt* _is_allowed1;
    ::sqlite3_stmt* _is_allowed2;
    ::sqlite3_stmt* _is_filtered;
    ::sqlite3_stmt* _promote;
    ::sqlite3_stmt* _demote;
    ::sqlite3_stmt* _timeout;
    ::sqlite3_stmt* _filter;
    ::sqlite3_stmt* _discard;
    ::opentelemetry::nostd::shared_ptr<::opentelemetry::trace::Tracer> _tracer;
};

} // detail
} // moderators
} // chat
} // ai

#include "impl/connection.ipp"

#endif
