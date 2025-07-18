#ifndef AI_CHAT_COMMANDS_ALLOW_IPP
#define AI_CHAT_COMMANDS_ALLOW_IPP

namespace ai {
namespace chat {
namespace commands {

template<typename Moderator>
allow<Moderator>::allow(Moderator &moderator)
    : _moderator{ moderator }
    , _parser{ R"([a-z]+)" } {

};

template<typename Moderator>
::std::string_view allow<Moderator>::execute(::std::string_view args) {
    if (!::RE2::FullMatch(args, _parser)) {
        return ::std::string_view{};
    }
    _moderator.allow(args)
    return args;
};

} // commands
} // chat
} // ai

#endif
