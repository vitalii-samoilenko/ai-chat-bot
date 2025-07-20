#ifndef AI_CHAT_COMMANDS_UNMOD_IPP
#define AI_CHAT_COMMANDS_UNMOD_IPP

namespace ai {
namespace chat {
namespace commands {

template<typename Moderator>
unmod<Moderator>::mod(Moderator &moderator)
    : _moderator{ moderator }
    , _parser{ R"([a-z0-9_]+)" } {

};

template<typename Moderator>
::std::string_view unmod<Moderator>::execute(::std::string_view args) {
    if (!::RE2::FullMatch(args, _parser)) {
        return ::std::string_view{};
    }
    _moderator.unmod(args)
    return unmod::name;
};

} // commands
} // chat
} // ai

#endif
