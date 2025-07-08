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
    twitch(const twitch&) = delete;
    twitch(twitch&&) = delete;

    ~twitch() = delete;

    twitch& operator=(const twitch&) = delete;
    twitch& operator=(twitch&&) = delete;

    class binding {
    public:
        binding() = delete;
        binding(const binding&) = delete;
        binding(binding&&) = default;

        ~binding() = default;

        binding& operator=(const binding&) = delete;
        binding& operator=(binding&&) = default;

    private:
        friend twitch;

        binding(typename ::ai::chat::histories::observable<History>::slot&& s_history, ::ai::chat::clients::observable<::ai::chat::clients::twitch>::slot&& s_client);

        typename ::ai::chat::histories::observable<History>::slot _s_history;
        ::ai::chat::clients::observable<::ai::chat::clients::twitch>::slot _s_client;
    };

    template<typename Moderator, typename... Commands>
    static binding bind(::ai::chat::histories::observable<History>& history, ::ai::chat::clients::observable<::ai::chat::clients::twitch>& client,
        Moderator& moderator, ::ai::chat::commands::executor<Commands...>& executor,
        const ::std::string& botname);
};

} // binders
} // chat
} // ai

#include "impl/twitch.ipp"

#endif
