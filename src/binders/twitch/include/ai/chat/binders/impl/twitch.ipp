#ifndef AI_CHAT_BINDERS_TWITCH_IPP
#define AI_CHAT_BINDERS_TWITCH_IPP

#include <utility>

#include "ai/chat/binders/twitch.hpp"

namespace ai {
namespace chat {
namespace binders {

template<typename History, typename Client>
twitch<History, Client>::binding::binding(History::slot_type&& history_slot, Client::slot_type&& client_slot)
    : _history_slot{ ::std::move(history_slot) }
    , _client_slot{ ::std::move(client_slot) } {

};

template<typename History, typename Client>
twitch<History, Client>::binding_type twitch<History, Client>::bind(History& history, Client& client) {
    History::slot_type history_slot{ history.subscribe<Client>() };
    history_slot.on_message([&client](const History::message_type& history_message)->void {
        const History::tag_type* p_username_tag{ nullptr };
        const History::tag_type* p_channel_tag{ nullptr };
        for (const History::tag_type& tag : history_message.tags) {
            if (tag.name == "user.name") {
                p_username_tag = &tag;
            } else if (tag.name == "channel") {
                p_channel_tag = &tag;
            }
        }
        Client::message_type client_message{
            p_username_tag->value,
            history_message.content,
            p_channel_tag->value
        };
        client.send(client_message);
    });
    Client::slot_type client_slot{ client.subscribe<History>() };
    client_slot.on_message([&history](const Client::message_type& client_message)->void {
        History::message_type history_message{
            {},
            client_message.content,
            {
                {"user.name", client_message.username},
                {"channel", client_message.channel}
            }
        };
        history.insert<Client>(history_message);
    });
    return { ::std::move(history_slot), ::std::move(client_slot) };
};

} // binders
} // chat
} // ai

#endif
