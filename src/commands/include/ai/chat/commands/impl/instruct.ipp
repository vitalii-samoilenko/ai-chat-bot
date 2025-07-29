#ifndef AI_CHAT_COMMANDS_INSTRUCT_IPP
#define AI_CHAT_COMMANDS_INSTRUCT_IPP

#include "ai/chat/commands/instruct.hpp"

namespace ai {
namespace chat {
namespace commands {

template<typename History>
instruct<History>::instruct(::ai::chat::histories::observable<History> &history)
    : _history{ history }
    , _tags{} {
    _tags.emplace_back("source", "command");
    _tags.emplace_back("system", "");
};

template<typename History>
::std::string_view instruct<History>::execute(::std::string_view args) {
    if (args.empty()) {
        return ::std::string_view{};
    }
    _history.template insert<instruct>(::ai::chat::histories::message{
        ::std::chrono::nanoseconds{},
        args,
        _tags
    });
    return instruct::name;
};

} // commands
} // chat
} // ai

#endif
