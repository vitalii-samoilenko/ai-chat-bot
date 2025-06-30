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

    using iterator_type = size_t;

    explicit sqlite(const ::std::string& filename);

    iterator_type is_moderator(const ::std::string& username);
    iterator_type is_allowed(const ::std::string& username1, const ::std::string& username2);
    iterator_type is_filtered(const ::std::string& content);

    iterator_type mod(const ::std::string& username);
    iterator_type unmod(const ::std::string& username);
    iterator_type allow(const ::std::string& username);
    iterator_type deny(const ::std::string& username);
    iterator_type timeout(const ::std::string& username, ::std::chrono::seconds until);
    iterator_type ban(const ::std::string& username);
    iterator_type unban(const ::std::string& username);
    iterator_type filter(const ::std::string& name, const ::std::string& pattern);
    iterator_type discard(const ::std::string& name);

private:
    class connection;

    ::std::unique_ptr<connection> _p_controller;
};

} // moderators
} // chat
} // ai

#include "impl/sqlite.ipp"

#endif
