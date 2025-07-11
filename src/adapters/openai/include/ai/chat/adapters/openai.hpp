#ifndef AI_CHAT_ADAPTERS_OPENAI_HPP
#define AI_CHAT_ADAPTERS_OPENAI_HPP

#include <chrono>
#include <memory>
#include <string>

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
    ::std::string content;
};

class openai {
public:
    openai() = delete;
    openai(const openai&) = delete;
    openai(openai&&) = delete;

    ~openai() = default;

    openai& operator=(const openai&) = delete;
    openai& operator=(openai&&) = delete;

    using iterator = size_t;

    openai(const ::std::string& address, ::std::chrono::milliseconds timeout,
        ::std::chrono::milliseconds delay);

    iterator insert(const message& message);
    message complete(const ::std::string& model, const ::std::string& key);

private:
    class connection;

    ::std::unique_ptr<connection> _context;
};

} // adapters
} // chat
} // ai

#include "impl/openai.ipp"

#endif
