#ifndef AI_CHAT_BINDERS_TWITCH_HPP
#define AI_CHAT_BINDERS_TWITCH_HPP

#include "ai/chat/clients/twitch.hpp"
#include "ai/chat/clients/handlers/observable.hpp"

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
        friend twitch;

    public:
        binding() = delete;
        binding(const binding&) = delete;
        binding(binding&&) = default;

        ~binding() = default;

        binding& operator=(const binding&) = delete;
        binding& operator=(binding&&) = default;

    private:
        binding(typename History::slot&& history_slot, typename Client::slot&& client_slot);

        typename History::slot _history_slot;
        typename Client::slot _client_slot;
    };

    template<typename Moderator>
    static binding bind(History& history, Client& client,
        Moderator& moderator,
        const ::std::string& botname);
};

} // binders
} // chat
} // ai

#include "impl/twitch.ipp"

#endif
