#ifndef AI_CHAT_HISTORIES_SQLITE_HPP
#define AI_CHAT_HISTORIES_SQLITE_HPP

#include <chrono>
#include <span>
#include <string_view>

namespace ai {
namespace chat {
namespace histories {

struct tag {
    ::std::string_view name;
    ::std::string_view value;
};
struct message {
    ::std::chrono::nanoseconds timestamp;
    ::std::string_view content;
    ::std::span<tag> tags;
};

class iterator;
class sqlite;

} // histories
} // chat
} // ai

#include "detail/connection.hpp"
#include "detail/scope.hpp"

namespace ai {
namespace chat {
namespace histories {

class iterator {
public:
    iterator() = delete;
    iterator(iterator const &other);
    iterator(iterator &&other);

    ~iterator();

    iterator & operator=(iterator const &other) = delete;
    iterator & operator=(iterator &&other) = delete;

    message operator*();
    iterator & operator++();
    bool operator==(iterator const &rhs) const;

    iterator operator+(ptrdiff_t rhs) const;
    iterator operator+(::std::chrono::nanoseconds rhs) const;
    ptrdiff_t operator-(iterator rhs) const;

    iterator & operator&=(tag rhs);

private:
    friend sqlite;

    template<typename... Args>
    explicit iterator(Args &&...args);

    detail::scope _target;
};

class sqlite {
public:
    sqlite() = delete;
    sqlite(sqlite const &other) = delete;
    sqlite(sqlite &&other) = delete;

    ~sqlite() = default;

    sqlite & operator=(sqlite const &other) = delete;
    sqlite & operator=(sqlite &&other) = delete;

    explicit sqlite(::std::string_view filename);

    iterator begin();
    iterator end();

    iterator lower_bound(::std::chrono::nanoseconds timestamp);

    iterator insert(message value);
    iterator erase(iterator pos);
    iterator erase(iterator first, iterator last);

private:
    detail::connection _chat;
};

} // histories
} // chat
} // ai

#include "impl/sqlite.ipp"

#endif
