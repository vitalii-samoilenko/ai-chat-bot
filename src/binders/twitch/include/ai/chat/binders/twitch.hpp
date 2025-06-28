#ifndef AI_CHAT_BINDERS_TWITCH_HPP
#define AI_CHAT_BINDERS_TWITCH_HPP

#include "ai/chat/clients/twitch/irc.hpp"
#include "ai/chat/clients/twitch/handlers/observable.hpp"

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
        binding(typename History::slot_type&& history_slot, typename Client::slot_type&& client_slot);

        typename History::slot_type _history_slot;
        typename Client::slot_type _client_slot;
    };

    using binding_type = binding;

    static binding_type bind(History& history, Client& client);
};

} // binders
} // chat
} // ai

#include "impl/twitch.ipp"

#endif
