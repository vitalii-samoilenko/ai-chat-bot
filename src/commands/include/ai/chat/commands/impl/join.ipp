#ifndef AI_CHAT_COMMANDS_JOIN_IPP
#define AI_CHAT_COMMANDS_JOIN_IPP

namespace ai {
namespace chat {
namespace commands {

template<typename Client>
join<Client>::join(Client &client)
    : _client{ client }
    , _parser{ R"([a-z0-9_]+)" } {

};

template<typename Client>
::std::string_view join<Client>::execute(::std::string_view args) {
    if (!::RE2::FullMatch(args, _parser)) {
        return ::std::string_view{};
    }
    _client.join(args);
    return join::name;
};

} // commands
} // chat
} // ai

#endif
