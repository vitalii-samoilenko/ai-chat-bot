#ifndef AI_CHAT_COMMANDS_EDIT_IPP
#define AI_CHAT_COMMANDS_EDIT_IPP

namespace ai {
namespace chat {
namespace commands {

template<typename History>
edit<History>::edit(::ai::chat::histories::observable<History> &history)
    : _history{ history }
    , _parser{ R"((?<timestamp>[0-9]+) (?<content>.*))" } {

};

template<typename History>
::std::string_view edit<History>::execute(::std::string_view args) {
    long long representation{};
    ::std::string_view content{};
    if (!::RE2::FullMatch(args, _parser,
            &representation, content)) {
        return ::std::string_view{};
    }
    ::std::chrono::nanoseconds timestamp{ representation };
    ::ai::chat::histories::observable_iterator<History> pos{ _history.lower_bound(timestamp) };
    if (timestamp < pos) {
        return ::std::string_view{};
    }
    pos = content;
    return edit::name;
};

} // commands
} // chat
} // ai

#endif
