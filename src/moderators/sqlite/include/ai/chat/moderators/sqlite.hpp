#ifndef AI_CHAT_MODERATORS_SQLITE_HPP
#define AI_CHAT_MODERATORS_SQLITE_HPP

#include <chrono>
#include <string_view>

namespace ai {
namespace chat {
namespace moderators {

using iterator = size_t;
class sqlite;

} // moderators
} // chat
} // ai

#include "detail/connection.hpp"

namespace ai {
namespace chat {
namespace moderators {

class sqlite {
public:
    sqlite() = delete;
    sqlite(sqlite const &) = delete;
    sqlite(sqlite &&) = delete;

    ~sqlite() = default;

    sqlite& operator=(sqlite const &) = delete;
    sqlite& operator=(sqlite &&) = delete;

    sqlite(::std::string_view filename, size_t length);

    iterator is_moderator(::std::string_view username);
    iterator is_allowed(::std::string_view username1, ::std::string_view username2);
    iterator is_filtered(::std::string_view content);

    iterator mod(::std::string_view username);
    iterator unmod(::std::string_view username);
    iterator allow(::std::string_view username);
    iterator deny(::std::string_view username);
    iterator timeout(::std::string_view username, ::std::chrono::seconds until);
    iterator ban(::std::string_view username);
    iterator unban(::std::string_view username);
    iterator filter(::std::string_view name, ::std::string_view pattern);
    iterator discard(::std::string_view name);

private:
    detail::connection _controller;
};

} // moderators
} // chat
} // ai

#include "impl/sqlite.ipp"

#endif
