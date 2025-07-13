#ifndef AI_CHAT_ADAPTERS_OPENAI_HPP
#define AI_CHAT_ADAPTERS_OPENAI_HPP

#include <chrono>
#include <memory>
#include <string_view>

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

class openai {
private:
    class connection;

public:
    openai() = delete;
    openai(openai const &) = delete;
    openai(openai &&) = delete;

    ~openai() = default;

    openai & operator=(openai const &) = delete;
    openai & operator=(openai &&) = delete;

    class iterator {
    public:
        iterator() = delete;
        iterator(iterator const &) = default;
        iterator(iterator &&) = default;

        ~iterator() = default;

        iterator & operator=(iterator const &) = delete;
        iterator & operator=(iterator &&) = delete;

        message operator*();
        iterator & operator++();
        iterator & operator+=(size_t rhs);
        bool operator==(iterator const &rhs) const;

    private:
        friend openai;

        iterator(connection *context, size_t pos);

        connection *_context;
        size_t _pos;
    };

    openai(::std::string_view address, ::std::chrono::milliseconds timeout,
        ::std::chrono::milliseconds delay, size_t limit);

    iterator begin();
    iterator end();

    void push_back(message const &value);
    void pop_back();

    iterator complete(::std::string_view model, ::std::string_view key);
    iterator erase(iterator first, iterator last);

private:
    ::std::unique_ptr<connection> _context;
};

} // adapters
} // chat
} // ai

#include "impl/openai.ipp"

#endif
