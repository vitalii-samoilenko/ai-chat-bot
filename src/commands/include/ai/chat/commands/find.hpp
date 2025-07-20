#ifndef AI_CHAT_COMMANDS_FIND_HPP
#define AI_CHAT_COMMANDS_FIND_HPP

#include <string>
#include <string_view>

#include "re2/re2.h"

#include "ai/chat/histories/observable.hpp"

namespace ai {
namespace chat {
namespace commands {

template<typename History, size_t Limit>
class find {
public:
    static char constexpr name[]{ "find" };

    find() = delete;
    find(find const &other) = delete;
    find(find &&other) = delete;

    ~find() = default;

    find & operator=(find const &other) = delete;
    find & operator=(find &&other) = delete;

    explicit find(::ai::chat::histories::observable<History> &history);

    ::std::string_view execute(::std::string_view args);

private:
    ::ai::chat::histories::observable<History> &_history;
    ::RE2 _range_parser;
    ::RE2 _tag_parser;
    ::std::string _buffer;
};

} // commands
} // chat
} // ai

#include "impl/find.ipp"

#endif
