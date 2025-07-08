#ifndef AI_CHAT_HISTORIES_SQLITE_HPP
#define AI_CHAT_HISTORIES_SQLITE_HPP

#include <chrono>
#include <memory>
#include <string>
#include <vector>

namespace ai {
namespace chat {
namespace histories {

struct tag {
    ::std::string name;
    ::std::string value;
};
struct message {
    ::std::chrono::nanoseconds timestamp;
    ::std::string content;
    ::std::vector<tag> tags;
};

class sqlite {
public:
    sqlite() = delete;
    sqlite(const sqlite&) = delete;
    sqlite(sqlite&&) = delete;

    ~sqlite() = default;

    sqlite& operator=(const sqlite&) = delete;
    sqlite& operator=(sqlite&&) = delete;

    using iterator = ::std::chrono::nanoseconds;

    explicit sqlite(const ::std::string& filename);

    iterator insert(const message& message);

private:
    class connection;

    ::std::unique_ptr<connection> _chat;
};

} // histories
} // chat
} // ai

#include "impl/sqlite.ipp"

#endif
