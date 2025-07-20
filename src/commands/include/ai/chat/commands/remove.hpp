#ifndef AI_CHAT_COMMANDS_REMOVE_HPP
#define AI_CHAT_COMMANDS_REMOVE_HPP

#include <string_view>

#include "re2/re2.h"

#include "ai/chat/histories/observable.hpp"

namespace ai {
namespace chat {
namespace commands {

template<typename History>
class remove {
public:
    static char constexpr name[]{ "remove" };

    remove() = delete;
    remove(remove const &other) = delete;
    remove(remove &&other) = delete;

    ~remove() = default;

    remove & operator=(remove const &other) = delete;
    remove & operator=(remove &&other) = delete;

    explicit remove(::ai::chat::histories::observable<History> &history);

    ::std::string_view execute(::std::string_view args);

private:
    ::ai::chat::histories::observable<History> &_history;
    ::RE2 _range_parser;
    ::RE2 _timestamp_parser;
};

} // commands
} // chat
} // ai

#include "impl/remove.ipp"

#endif
