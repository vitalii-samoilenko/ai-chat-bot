#ifndef AI_CHAT_MODERATORS_SQLITE_IPP
#define AI_CHAT_MODERATORS_SQLITE_IPP

#include <limits>

namespace ai {
namespace chat {
namespace moderators {

sqlite::sqlite(::std::string_view filename, size_t length)
    : _controller{} {
    _controller._filename = filename;
    _controller._length = static_cast<::sqlite3_int64>(length);
    _controller.on_init();
};

iterator sqlite::is_moderator(::std::string_view username) {
    ::opentelemetry::nostd::shared_ptr<::opentelemetry::trace::Span> span{
        _controller._tracer->StartSpan("is_moderator")
    };
    return _controller.on_is_allowed(username, detail::connection::role::moderator | detail::connection::role::administrator,
        ::std::numeric_limits<::sqlite3_int64>::max(),
        span);
};
iterator sqlite::is_allowed(::std::string_view username1, ::std::string_view username2) {
    ::opentelemetry::nostd::shared_ptr<::opentelemetry::trace::Span> span{
        _controller._tracer->StartSpan("is_allowed")
    };
    auto now = ::std::chrono::steady_clock::now();
    return _controller.on_is_allowed(username1, username2, detail::connection::role::interlocutor,
        ::std::chrono::duration_cast<::std::chrono::seconds>(now.time_since_epoch()).count(),
        span);
};
iterator sqlite::is_filtered(::std::string_view content) {
    ::opentelemetry::nostd::shared_ptr<::opentelemetry::trace::Span> span{
        _controller._tracer->StartSpan("is_filtered")
    };
    return _controller.on_is_filtered(content,
        span);
};

iterator sqlite::admin(::std::string_view username) {
    ::opentelemetry::nostd::shared_ptr<::opentelemetry::trace::Span> span{
        _controller._tracer->StartSpan("admin")
    };
    return _controller.on_promote(username, detail::connection::role::administrator,
        span);
};
iterator sqlite::mod(::std::string_view username) {
    ::opentelemetry::nostd::shared_ptr<::opentelemetry::trace::Span> span{
        _controller._tracer->StartSpan("mod")
    };
    return _controller.on_promote(username, detail::connection::role::moderator,
        span);
};
iterator sqlite::unmod(::std::string_view username) {
    ::opentelemetry::nostd::shared_ptr<::opentelemetry::trace::Span> span{
        _controller._tracer->StartSpan("unmod")
    };
    return _controller.on_demote(username, detail::connection::role::moderator,
        span);
};
iterator sqlite::allow(::std::string_view username) {
    ::opentelemetry::nostd::shared_ptr<::opentelemetry::trace::Span> span{
        _controller._tracer->StartSpan("allow")
    };
    return _controller.on_promote(username, detail::connection::role::interlocutor,
        span);
};
iterator sqlite::deny(::std::string_view username) {
    ::opentelemetry::nostd::shared_ptr<::opentelemetry::trace::Span> span{
        _controller._tracer->StartSpan("deny")
    };
    return _controller.on_demote(username, detail::connection::role::interlocutor,
        span);
};
iterator sqlite::timeout(::std::string_view username, ::std::chrono::seconds until) {
    ::opentelemetry::nostd::shared_ptr<::opentelemetry::trace::Span> span{
        _controller._tracer->StartSpan("timeout")
    };
    return _controller.on_timeout(username, until.count(),
        span);
};
iterator sqlite::ban(::std::string_view username) {
    ::opentelemetry::nostd::shared_ptr<::opentelemetry::trace::Span> span{
        _controller._tracer->StartSpan("ban")
    };
    return _controller.on_timeout(username, ::std::numeric_limits<::sqlite3_int64>::max(),
        span);
};
iterator sqlite::unban(::std::string_view username) {
    ::opentelemetry::nostd::shared_ptr<::opentelemetry::trace::Span> span{
        _controller._tracer->StartSpan("unban")
    };
    return _controller.on_timeout(username, 0,
        span);
};
iterator sqlite::filter(::std::string_view name, ::std::string_view pattern) {
    ::opentelemetry::nostd::shared_ptr<::opentelemetry::trace::Span> span{
        _controller._tracer->StartSpan("filter")
    };
    return _controller.on_filter(name, pattern,
        span);
};
iterator sqlite::discard(::std::string_view name) {
    ::opentelemetry::nostd::shared_ptr<::opentelemetry::trace::Span> span{
        _controller._tracer->StartSpan("discard")
    };
    return _controller.on_discard(name,
        span);
};

} // moderators
} // chat
} // ai

#endif
