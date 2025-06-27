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
    role role;
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

    using iterator_type = size_t;
    using role_type = role;
    using message_type = message;

    openai(const ::std::string& address, ::std::chrono::milliseconds timeout);

    iterator_type insert(const message_type& message);
    message_type complete(const ::std::string& model, const ::std::string& key);

private:
    class connection;

    ::std::unique_ptr<connection> _p_context;
};

} // adapters
} // chat
} // ai

#include "impl/openai.ipp"

#endif
