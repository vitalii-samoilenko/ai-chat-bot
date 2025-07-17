#ifndef AI_CHAT_BINDERS_TWITCH_HPP
#define AI_CHAT_BINDERS_TWITCH_HPP

#include "ai/chat/clients/twitch.hpp"
#include "ai/chat/clients/observable.hpp"
#include "ai/chat/commands/executor.hpp"
#include "ai/chat/histories/observable.hpp"

namespace ai {
namespace chat {
namespace binders {

template<typename History>
class twitch {
public:
    twitch() = delete;
    twitch(twitch const &other) = delete;
    twitch(twitch &&other) = delete;

    ~twitch() = delete;

    twitch & operator=(twitch const &other) = delete;
    twitch & operator=(twitch &&other) = delete;

    class binding {
    public:
        binding() = delete;
        binding(binding const &other) = delete;
        binding(binding &&other) = default;

        ~binding() = default;

        binding & operator=(binding const &other) = delete;
        binding & operator=(binding &&other) = default;

    private:
        friend twitch;

        binding(::ai::chat::histories::slot<History> &&s_history, ::ai::chat::clients::slot<::ai::chat::clients::twitch> &&s_client);

        ::ai::chat::histories::slot<History> _s_history;
        ::ai::chat::clients::slot<::ai::chat::clients::twitch> _s_client;
    };

    template<typename Moderator, typename... Commands>
    static binding bind(::ai::chat::histories::observable<History> &history, ::ai::chat::clients::observable<::ai::chat::clients::twitch> &client,
        Moderator &moderator, ::ai::chat::commands::executor<Commands...> &executor,
        ::std::string_view botname);
};

} // binders
} // chat
} // ai

#include "impl/twitch.ipp"

#endif
