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
    const ::std::string& model, const ::std::string& key,
    const ::std::string& pattern, size_t retries, const ::std::string& apology,
    const ::std::string& botname) {
    auto history_slot = history.subscribe<Adapter>();
    history_slot.on_message([&history, &adapter, &moderator, model, key, pattern, retries, apology, botname](const ::ai::chat::histories::message& history_message)->void {
        const ::ai::chat::histories::tag* p_username_tag{ nullptr };
        const ::ai::chat::histories::tag* p_channel_tag{ nullptr };
        for (const ::ai::chat::histories::tag& tag : history_message.tags) {
            if (tag.name == "user.name") {
                p_username_tag = &tag;
            } else if (tag.name == "channel") {
                p_channel_tag = &tag;
            }
        }
        const char USERNAME[]{ "{username}" };
        const char CONTENT[]{ "{content}" };
        if (p_username_tag) {
            ::ai::chat::adapters::message adapter_message{ ::ai::chat::adapters::role::user, pattern };
            {
                size_t pos{ adapter_message.content.find(USERNAME) };
                if (!(pos == ::std::string::npos)) {
                    adapter_message.content.replace(pos, ::std::size(USERNAME) - 1, p_username_tag->value);
                }
            }
            {
                size_t pos{ adapter_message.content.find(CONTENT) };
                if (!(pos == ::std::string::npos)) {
                    adapter_message.content.replace(pos, ::std::size(CONTENT) - 1, history_message.content);
                }
            }
            adapter.insert(adapter_message);
        } else {
            ::ai::chat::adapters::message adapter_message{ ::ai::chat::adapters::role::system, history_message.content };
            adapter.insert(adapter_message);
            return;
        }
        ::ai::chat::adapters::message adapter_completion{ adapter.complete(model, key) };
        for (size_t left{ retries }; moderator.is_filtered(adapter_completion.content); --left) {
            if (left) {
                adapter_completion = adapter.complete(model, key);
            } else {
                adapter_completion.content = apology;
                {
                    size_t pos{ adapter_completion.content.find(USERNAME) };
                    if (!(pos == ::std::string::npos)) {
                        adapter_completion.content.replace(pos, ::std::size(USERNAME) - 1, p_username_tag->value);
                    }
                }
                break;
            }
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
