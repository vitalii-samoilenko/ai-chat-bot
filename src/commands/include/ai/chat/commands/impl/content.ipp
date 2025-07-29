#ifndef AI_CHAT_COMMANDS_CONTENT_IPP
#define AI_CHAT_COMMANDS_CONTENT_IPP

#include "ai/chat/commands/content.hpp"

namespace ai {
namespace chat {
namespace commands {

template<typename History>
content<History>::content(::ai::chat::histories::observable<History> &history)
    : _history{ history }
    , _parser{ R"((?<timestamp>[0-9]+))" }
    , _buffer{} {

};

template<typename History>
::std::string_view content<History>::execute(::std::string_view args) {
    long long representation{};
    if (!::RE2::FullMatch(args, _parser,
            &representation)) {
        return ::std::string_view{};
    }
    ::std::chrono::nanoseconds timestamp{ representation };
    ::ai::chat::histories::observable_iterator<History> pos{ _history.lower_bound(timestamp) };
    if (pos > timestamp) {
        return ::std::string_view{};
    }
    ::ai::chat::histories::message message{ *pos };
    _buffer = message.content;
    return _buffer;
};

} // commands
} // chat
} // ai

#endif
