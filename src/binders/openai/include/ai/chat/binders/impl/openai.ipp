#ifndef AI_CHAT_BINDERS_OPENAI_IPP
#define AI_CHAT_BINDERS_OPENAI_IPP

#include <utility>

#include "ai/chat/binders/openai.hpp"

namespace ai {
namespace chat {
namespace binders {

template<typename History, typename Adapter>
openai<History, Adapter>::binding::binding(typename History::slot&& history_slot)
    : _history_slot{ ::std::move(history_slot) } {

};

template<typename History, typename Adapter>
template<typename Moderator>
typename openai<History, Adapter>::binding openai<History, Adapter>::bind(History& history, Adapter& adapter,
    Moderator& moderator,
    const ::std::string& botname, const ::std::string& model, const ::std::string& key) {
    auto history_slot = history.subscribe<Adapter>();
    history_slot.on_message([&history, &adapter, &moderator, botname, model, key](const ::ai::chat::histories::message& history_message)->void {
        ::ai::chat::adapters::message adapter_message{
            ::ai::chat::adapters::role::system,
            history_message.content
        };
        const ::ai::chat::histories::tag* p_channel_tag{ nullptr };
        for (const ::ai::chat::histories::tag& tag : history_message.tags) {
            if (tag.name == "user.name") {
                adapter_message.role = tag.value == botname
                    ? ::ai::chat::adapters::role::assistant
                    : ::ai::chat::adapters::role::user;
            } else if (tag.name == "channel") {
                p_channel_tag = &tag;
            }
        }
        adapter.insert(adapter_message);
        ::ai::chat::adapters::message adapter_completion{ adapter.complete(model, key) };
        if (moderator.is_filtered(adapter_completion.content)) {
            return;
        }
        adapter.insert(adapter_completion);
        ::ai::chat::histories::message history_completion{ 
            {},
            adapter_completion.content,
            p_channel_tag
                ? ::std::initializer_list<::ai::chat::histories::tag>{
                    {"user.name", botname},
                    *p_channel_tag
                }
                : ::std::initializer_list<::ai::chat::histories::tag>{
                    {"user.name", botname}
                }
        };
        history.insert<Adapter>(history_completion);
    });
    return binding{ ::std::move(history_slot) };
};

} // binders
} // chat
} // ai

#endif
