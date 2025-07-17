#ifndef AI_CHAT_BINDERS_TWITCH_IPP
#define AI_CHAT_BINDERS_TWITCH_IPP

#include <string>
#include <utility>
#include <vector>

namespace ai {
namespace chat {
namespace binders {

template<typename History>
twitch<History>::binding::binding(::ai::chat::histories::slot<History> &&s_history, ::ai::chat::clients::slot<::ai::chat::clients::twitch> &&s_client)
    : _s_history{ ::std::move(s_history) }
    , _s_client{ ::std::move(s_client) } {

};

template<typename History>
template<typename Moderator, typename... Commands>
twitch<History>::binding twitch<History>::bind(::ai::chat::histories::observable<History> &history, ::ai::chat::clients::observable<::ai::chat::clients::twitch> &client,
    Moderator &moderator, ::ai::chat::commands::executor<Commands...> &executor,
    ::std::string_view botname) {
    ::ai::chat::histories::slot<History> s_history{ history.subscribe<::ai::chat::clients::observable<::ai::chat::clients::twitch>>() };
    s_history.on_message([&client
    ](::ai::chat::histories::iterator history_pos)->void {
        ::ai::chat::histories::message history_message{ *history_pos };
        ::ai::chat::histories::tag const *username_tag{ nullptr };
        ::ai::chat::histories::tag const *channel_tag{ nullptr };
        for (::ai::chat::histories::tag const &tag : history_message.tags) {
            if (tag.name == "user.name") {
                username_tag = &tag;
            } else if (tag.name == "channel") {
                channel_tag = &tag;
            }
        }
        client.send(::ai::chat::clients::message{
            username_tag->value,
            history_message.content,
            channel_tag->value
        });
    });
    ::ai::chat::clients::slot<::ai::chat::clients::twitch> s_client{ client.subscribe<::ai::chat::histories::observable<History>>() };
    s_client.on_message([&history, &moderator,
        botname = ::std::string{ botname }
    ](::ai::chat::clients::message client_message)->void {
        if (client_message.content.find("@" + botname) == ::std::string::npos) {
            return;
        }
        if (!moderator.is_allowed(botname, client_message.username)) {
            return;
        }
        ::std::vector<::ai::chat::histories::tag> tags{
            ::ai::chat::histories::tag{ "user.name", client_message.username },
            ::ai::chat::histories::tag{ "channel", client_message.channel },
        };
        history.insert<::ai::chat::clients::observable<::ai::chat::clients::twitch>>(::ai::chat::histories::message{
            {},
            client_message.content,
            tags
        });
    });
    s_client.on_command([&client,
        &moderator, &executor,
        botname = ::std::string{ botname }
    ](::ai::chat::clients::command client_command)->void {
        if (!(client_command.channel == botname)) {
            return;
        }
        if (!moderator.is_moderator(client_command.username)) {
            return;
        }
        client.send(::ai::chat::clients::message{
            botname,
            executor.execute(client_command.name, client_command.args),
            client_command.channel
        });
    });
    return binding{ ::std::move(s_history), ::std::move(s_client) };
};

} // binders
} // chat
} // ai

#endif
