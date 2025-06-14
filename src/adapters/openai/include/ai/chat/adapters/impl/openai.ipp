#ifndef AI_CHAT_ADAPTERS_OPENAI_IPP
#define AI_CHAT_ADAPTERS_OPENAI_IPP

#include "ai/chat/adapters/openai.hpp"

namespace ai {
namespace chat {
namespace adapters {

template<typename Range>
template<typename... Args>
openai<Range>::openai(const ::std::string& model, const Range& messages, Args&& ...args)
    : _context{ model, messages }
    , _client{ ::std::forward<Args>(args)... } {

};

template<typename Range>
::std::pair<::std::string, size_t> openai<Range>::complete() const {
    ::openai::completion_result result{ _client.complete(_context) };
    return ::std::make_pair(
        result.choices[0].message.content,
        result.usage.total_tokens);
};

} // adapters
} // chat
} // ai

#endif
