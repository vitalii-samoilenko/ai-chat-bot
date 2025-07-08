#ifndef AI_CHAT_BINDERS_TWITCH_HPP
#define AI_CHAT_BINDERS_TWITCH_HPP

#include "ai/chat/clients/twitch.hpp"
#include "ai/chat/clients/observable.hpp"

namespace ai {
namespace chat {
namespace binders {

template<typename History, typename Client>
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

        binding(typename History::slot&& s_history, typename Client::slot&& s_client);

        typename History::slot _s_history;
        typename Client::slot _s_client;
    };

    template<typename Moderator, typename Executor>
    static binding bind(History& history, Client& client,
        Moderator& moderator, Executor& executor,
        const ::std::string& botname);
};

} // binders
} // chat
} // ai

#include "impl/twitch.ipp"

#endif
