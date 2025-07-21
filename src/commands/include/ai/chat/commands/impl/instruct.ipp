#ifndef AI_CHAT_COMMANDS_INSTRUCT_IPP
#define AI_CHAT_COMMANDS_INSTRUCT_IPP

#include <vector>

namespace ai {
namespace chat {
namespace commands {

template<typename History>
instruct<History>::instruct(::ai::chat::histories::observable<History> &history)
    : _history{ history } {

};

template<typename History>
::std::string_view instruct<History>::execute(::std::string_view args) {
    if (args.empty()) {
        return ::std::string_view{};
    }
    ::std::vector<::ai::chat::histories::tag> tags{};
    tags.emplace_back("source", "command");
    tags.emplace_back("system", "");
    _history.template insert<instruct>(::ai::chat::histories::message{
        ::std::chrono::nanoseconds{},
        args,
        tags
    });
    return instruct::name;
};

} // commands
} // chat
} // ai

#endif
