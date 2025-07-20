#ifndef AI_CHAT_COMMANDS_ALLOW_IPP
#define AI_CHAT_COMMANDS_ALLOW_IPP

namespace ai {
namespace chat {
namespace commands {

template<typename Moderator>
allow<Moderator>::allow(Moderator &moderator)
    : _moderator{ moderator }
    , _parser{ R"([a-z0-9_]+)" } {

};

template<typename Moderator>
::std::string_view allow<Moderator>::execute(::std::string_view args) {
    if (!::RE2::FullMatch(args, _parser)) {
        return ::std::string_view{};
    }
    _moderator.allow(args)
    return allow::name;
};

} // commands
} // chat
} // ai

#endif
