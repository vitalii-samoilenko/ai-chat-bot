#ifndef AI_CHAT_ADAPTERS_OPENAI_HPP
#define AI_CHAT_ADAPTERS_OPENAI_HPP

#include <chrono>
#include <string_view>

#include "detail/connection.hpp"
#include "detail/scope.hpp"

namespace ai {
namespace chat {
namespace adapters {

enum class role {
    system,
    user,
    assistant
};
struct message {
    ::ai::chat::adapters::role role;
    ::std::string_view content;
};

class openai;

class iterator {
public:
    iterator() = delete;
    iterator(iterator const &other);
    iterator(iterator &&other);

    ~iterator() = default;

    iterator & operator=(iterator const &other);
    iterator & operator=(iterator &&other);

    message operator*();
    iterator & operator++();
    iterator operator+(size_t rhs);
    bool operator==(iterator const &rhs) const;

private:
    friend openai;

    template<typename... Args>
    explicit iterator(Args &&...args);

    detail::scope _target;
};

class openai {
public:
    openai() = delete;
    openai(openai const &other) = delete;
    openai(openai &&other) = delete;

    ~openai() = default;

    openai & operator=(openai const &other) = delete;
    openai & operator=(openai &&other) = delete;

    openai(::std::string_view address, ::std::chrono::milliseconds timeout,
        ::std::chrono::milliseconds delay, size_t limit);

    iterator begin();
    iterator end();

    void push_back(message const &value);
    void pop_back();

    iterator complete(::std::string_view model, ::std::string_view key);
    iterator erase(iterator first, iterator last);

private:
    detail::connection _context;
};

} // adapters
} // chat
} // ai

#include "impl/openai.ipp"

#endif
