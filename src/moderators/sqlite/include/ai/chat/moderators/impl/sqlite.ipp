#ifndef AI_CHAT_MODERATORS_SQLITE_IPP
#define AI_CHAT_MODERATORS_SQLITE_IPP

#include <limits>

#include "ai/chat/moderators/sqlite.hpp"

namespace ai {
namespace chat {
namespace moderators {

sqlite::sqlite(::std::string_view filename)
    : _controller{} {
    _controller._filename = filename;
    _controller.on_init();
};

iterator sqlite::is_moderator(::std::string_view username) {
    START_SPAN(span, "is_moderator", _controller)
    return _controller.on_is_allowed(username, detail::connection::role::moderator | detail::connection::role::administrator,
        ::std::numeric_limits<::sqlite3_int64>::max()
        PROPAGATE_SPAN(span));
};
iterator sqlite::is_allowed(::std::string_view username1, ::std::string_view username2) {
    START_SPAN(span, "is_allowed", _controller)
    auto now = ::std::chrono::utc_clock::now();
    return _controller.on_is_allowed(username1, username2, detail::connection::role::interlocutor,
        ::std::chrono::duration_cast<::std::chrono::seconds>(now.time_since_epoch()).count()
        PROPAGATE_SPAN(span));
};
iterator sqlite::is_filtered(::std::string_view content) {
    START_SPAN(span, "is_filtered", _controller)
    return _controller.on_is_filtered(content
        PROPAGATE_SPAN(span));
};

iterator sqlite::admin(::std::string_view username) {
    START_SPAN(span, "admin", _controller)
    return _controller.on_promote(username, detail::connection::role::administrator
        PROPAGATE_SPAN(span));
};
iterator sqlite::mod(::std::string_view username) {
    START_SPAN(span, "mod", _controller)
    return _controller.on_promote(username, detail::connection::role::moderator
        PROPAGATE_SPAN(span));
};
iterator sqlite::unmod(::std::string_view username) {
    START_SPAN(span, "unmod", _controller)
    return _controller.on_demote(username, detail::connection::role::moderator
        PROPAGATE_SPAN(span));
};
iterator sqlite::allow(::std::string_view username) {
    START_SPAN(span, "allow", _controller)
    return _controller.on_promote(username, detail::connection::role::interlocutor
        PROPAGATE_SPAN(span));
};
iterator sqlite::deny(::std::string_view username) {
    START_SPAN(span, "deny", _controller)
    return _controller.on_demote(username, detail::connection::role::interlocutor
        PROPAGATE_SPAN(span));
};
iterator sqlite::timeout(::std::string_view username, ::std::chrono::seconds until) {
    START_SPAN(span, "timeout", _controller)
    return _controller.on_timeout(username, until.count()
        PROPAGATE_SPAN(span));
};
iterator sqlite::ban(::std::string_view username) {
    START_SPAN(span, "ban", _controller)
    return _controller.on_timeout(username, ::std::numeric_limits<::sqlite3_int64>::max()
        PROPAGATE_SPAN(span));
};
iterator sqlite::unban(::std::string_view username) {
    START_SPAN(span, "unban", _controller)
    return _controller.on_timeout(username, 0
        PROPAGATE_SPAN(span));
};
iterator sqlite::filter(::std::string_view name, ::std::string_view pattern) {
    START_SPAN(span, "filter", _controller)
    return _controller.on_filter(name, pattern
        PROPAGATE_SPAN(span));
};
iterator sqlite::discard(::std::string_view name) {
    START_SPAN(span, "discard", _controller)
    return _controller.on_discard(name
        PROPAGATE_SPAN(span));
};

} // moderators
} // chat
} // ai

#endif
