#ifndef AI_CHAT_COMMANDS_CONTENT_HPP
#define AI_CHAT_COMMANDS_CONTENT_HPP

#include <string>
#include <string_view>

#include "re2/re2.h"

#include "ai/chat/histories/observable.hpp"

namespace ai {
namespace chat {
namespace commands {

template<typename History>
class content {
public:
    static char constexpr name[]{ "content" };

    content() = delete;
    content(content const &other) = delete;
    content(content &&other) = delete;

    ~content() = default;

    content & operator=(content const &other) = delete;
    content & operator=(content &&other) = delete;

    explicit content(::ai::chat::histories::observable<History> &history);

    ::std::string_view execute(::std::string_view args);

private:
    ::ai::chat::histories::observable<History> &_history;
    ::RE2 _parser;
    ::std::string _buffer;
};

} // commands
} // chat
} // ai

#include "impl/content.ipp"

#endif
