#ifndef AI_CHAT_BINDERS_OPENAI_IPP
#define AI_CHAT_BINDERS_OPENAI_IPP

#include <utility>

#include "ai/chat/binders/openai.hpp"

namespace ai {
namespace chat {
namespace binders {

template<typename History, typename Adapter>
openai<History, Adapter>::binding::binding(typename History::slot_type&& history_slot)
    : _history_slot{ ::std::move(history_slot) } {

};

template<typename History, typename Adapter>
typename openai<History, Adapter>::binding_type openai<History, Adapter>::bind(History& history, Adapter& adapter,
    const ::std::string& botname, const ::std::string& model, const ::std::string& key) {
    History::slot_type history_slot{ history.subscribe<Adapter>() };
    history_slot.on_message([&history, &adapter, botname, model, key](const History::message_type& history_message)->void {
        Adapter::message_type adapter_message{
            Adapter::role_type::system,
            history_message.content
        };
        const History::tag_type* p_channel_tag{ nullptr };
        for (const History::tag_type& tag : history_message.tags) {
            if (tag.name == "user.name") {
                adapter_message.role = tag.value == botname
                    ? Adapter::role_type::assistant
                    : Adapter::role_type::user;
            } else if (tag.name == "channel") {
                p_channel_tag = &tag;
            }
        }
        adapter.insert(adapter_message);
        Adapter::message_type adapter_completion{ adapter.complete(model, key) };
        adapter.insert(adapter_completion);
        History::message_type history_completion{ 
            {},
            adapter_completion.content,
            p_channel_tag
                ? ::std::initializer_list<History::tag_type>{
                    {"user.name", botname},
                    *p_channel_tag
                }
                : ::std::initializer_list<History::tag_type>{
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
