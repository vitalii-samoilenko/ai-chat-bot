#ifndef AI_CHAT_MODERATORS_SQLITE_HPP
#define AI_CHAT_MODERATORS_SQLITE_HPP

#include <chrono>
#include <memory>
#include <string>

namespace ai {
namespace chat {
namespace moderators {

class sqlite {
public:
    sqlite() = delete;
    sqlite(const sqlite&) = delete;
    sqlite(sqlite&&) = delete;

    ~sqlite() = default;

    sqlite& operator=(const sqlite&) = delete;
    sqlite& operator=(sqlite&&) = delete;

    using iterator = size_t;

    sqlite(const ::std::string& filename, size_t length);

    iterator is_moderator(const ::std::string& username);
    iterator is_allowed(const ::std::string& username1, const ::std::string& username2);
    iterator is_filtered(const ::std::string& content);

    iterator mod(const ::std::string& username);
    iterator unmod(const ::std::string& username);
    iterator allow(const ::std::string& username);
    iterator deny(const ::std::string& username);
    iterator timeout(const ::std::string& username, ::std::chrono::seconds until);
    iterator ban(const ::std::string& username);
    iterator unban(const ::std::string& username);
    iterator filter(const ::std::string& name, const ::std::string& pattern);
    iterator discard(const ::std::string& name);

private:
    class connection;

    ::std::unique_ptr<connection> _controller;
};

} // moderators
} // chat
} // ai

#include "impl/sqlite.ipp"

#endif
