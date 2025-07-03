#ifndef AI_CHAT_BINDERS_TWITCH_IPP
#define AI_CHAT_BINDERS_TWITCH_IPP

#include <utility>

#include "ai/chat/binders/twitch.hpp"

namespace ai {
namespace chat {
namespace binders {

template<typename History, typename Client>
twitch<History, Client>::binding::binding(typename History::slot&& history_slot, typename Client::slot&& client_slot)
    : _history_slot{ ::std::move(history_slot) }
    , _client_slot{ ::std::move(client_slot) } {

};

template<typename History, typename Client>
template<typename Moderator, typename Executor>
typename twitch<History, Client>::binding twitch<History, Client>::bind(History& history, Client& client,
    Moderator& moderator, Executor& executor,
    const ::std::string& botname) {
    auto history_slot = history.subscribe<Client>();
    history_slot.on_message([&client](const ::ai::chat::histories::message& history_message)->void {
        const ::ai::chat::histories::tag* p_username_tag{ nullptr };
        const ::ai::chat::histories::tag* p_channel_tag{ nullptr };
        for (const ::ai::chat::histories::tag& tag : history_message.tags) {
            if (tag.name == "user.name") {
                p_username_tag = &tag;
            } else if (tag.name == "channel") {
                p_channel_tag = &tag;
            }
        }
        ::ai::chat::clients::message client_message{
            p_username_tag->value,
            history_message.content,
            p_channel_tag->value
        };
        client.send(client_message);
    });
    auto client_slot = client.subscribe<History>();
    client_slot.on_message([&history, &moderator, botname](const ::ai::chat::clients::message& client_message)->void {
        if (client_message.content.find("@" + botname) == ::std::string::npos) {
            return;
        }
        if (!moderator.is_allowed(botname, client_message.username)) {
            return;
        }
        ::ai::chat::histories::message history_message{
            {},
            client_message.content,
            {
                {"user.name", client_message.username},
                {"channel", client_message.channel}
            }
        };
        history.insert<Client>(history_message);
    });
    client_slot.on_command([&client, &moderator, &executor, botname](const ::ai::chat::clients::command& client_command)->void {
        if (!(client_command.channel == botname)) {
            return;
        }
        if (!moderator.is_moderator(client_command.username)) {
            return;
        }
        ::ai::chat::clients::message client_message{
            botname,
            executor.execute(client_command.name, client_command.args),
            client_command.channel
        };
        client.send(client_message);
    });
    return binding{ ::std::move(history_slot), ::std::move(client_slot) };
};

} // binders
} // chat
} // ai

#endif
