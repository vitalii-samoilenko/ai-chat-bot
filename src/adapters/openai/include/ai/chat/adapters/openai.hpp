#ifndef AI_CHAT_ADAPTERS_OPENAI_HPP
#define AI_CHAT_ADAPTERS_OPENAI_HPP

#include <string>
#include <utility>

#include "openai/client.hpp"

namespace ai {
namespace chat {
namespace adapters {

template<typename Range>
class openai {
public:
    openai() = delete;
    openai(const openai&) = delete;
    openai(openai&&) = delete;

    ~openai() = default;

    openai& operator=(const openai&) = delete;
    openai& operator=(openai&&) = delete;

    template<typename... Args>
    openai(const ::std::string& model, const Range& messages, Args&& ...args);

    ::std::pair<::std::string, size_t> complete() const;

private:
    ::openai::completion_context<const Range&> _context;
    ::openai::client _client;
};

} // adapters
} // chat
} // ai

#include "impl/openai.ipp"

#endif
