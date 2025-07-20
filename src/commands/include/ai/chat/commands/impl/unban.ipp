#ifndef AI_CHAT_COMMANDS_UNBAN_IPP
#define AI_CHAT_COMMANDS_UNBAN_IPP

namespace ai {
namespace chat {
namespace commands {

template<typename Moderator>
unban<Moderator>::unban(Moderator &moderator)
    : _moderator{ moderator }
    , _parser{ R"([a-z0-9_]+)" } {

};

template<typename Moderator>
::std::string_view unban<Moderator>::execute(::std::string_view args) {
    if (!::RE2::FullMatch(args, _parser)) {
        return ::std::string_view{};
    }
    _moderator.unban(args);
    return unban::name;
};

} // commands
} // chat
} // ai

#endif
