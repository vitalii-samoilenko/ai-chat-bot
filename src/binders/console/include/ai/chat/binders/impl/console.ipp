#ifndef AI_CHAT_BINDERS_CONSOLE_IPP
#define AI_CHAT_BINDERS_CONSOLE_IPP

#include <string>
#include <utility>
#include <vector>

#include "ai/chat/binders/console.hpp"

namespace ai {
namespace chat {
namespace binders {

template<typename History>
console<History>::binding::binding(::ai::chat::histories::slot<History> &&s_history, ::ai::chat::clients::slot<::ai::chat::clients::console> &&s_client)
    : _s_history{ ::std::move(s_history) }
    , _s_client{ ::std::move(s_client) } {

};

template<typename History>
template<typename Moderator, typename... Commands>
console<History>::binding console<History>::bind(::ai::chat::histories::observable<History> &history, ::ai::chat::clients::observable<::ai::chat::clients::console> &client,
    Moderator &moderator, ::ai::chat::commands::executor<Commands...> &executor,
    ::std::string_view botname) {
    ::ai::chat::histories::slot<History> s_history{ history.template subscribe<::ai::chat::clients::observable<::ai::chat::clients::console>>() };
    s_history.on_message([&client,
        botname = ::std::string{ botname }
    ](::ai::chat::histories::observable_iterator<History> history_pos)->void {
        ::ai::chat::histories::message history_message{ *history_pos };
        ::ai::chat::histories::tag const *username_tag{ nullptr };
        for (::ai::chat::histories::tag const &tag : history_message.tags) {
            if (tag.name == "user.name") {
                username_tag = &tag;
                break;
            }
        }
        if (!username_tag) {
            return;
        }
        if (!(botname == username_tag->value)) {
            return;
        }
        client.send(::ai::chat::clients::message{
            username_tag->value,
            history_message.content
        });
    });
    ::ai::chat::clients::slot<::ai::chat::clients::console> s_client{ client.subscribe<::ai::chat::histories::observable<History>>() };
    s_client.on_message([&history, &moderator,
        botname = ::std::string{ botname }
    ](::ai::chat::clients::message client_message)->void {
        if (!moderator.is_allowed(botname, client_message.username)) {
            return;
        }
        ::std::vector<::ai::chat::histories::tag> tags{};
        tags.emplace_back("source", "console");
        tags.emplace_back("user.name", client_message.username);
        history.template insert<::ai::chat::clients::observable<::ai::chat::clients::console>>(::ai::chat::histories::message{
            ::std::chrono::nanoseconds{},
            client_message.content,
            tags
        });
    });
    s_client.on_command([&client,
        &moderator, &executor,
        botname = ::std::string{ botname }
    ](::ai::chat::clients::command client_command)->void {
        if (!moderator.is_moderator(client_command.username)) {
            return;
        }
        ::std::string_view result{ executor.execute(client_command.name, client_command.args) };
        if (result.empty()) {
            return;
        }
        client.send(::ai::chat::clients::message{
            botname,
            result
        });
    });
    return binding{ ::std::move(s_history), ::std::move(s_client) };
};

} // binders
} // chat
} // ai

#endif
