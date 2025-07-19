#ifndef AI_CHAT_BINDERS_OPENAI_IPP
#define AI_CHAT_BINDERS_OPENAI_IPP

#include <string>
#include <utility>
#include <vector>

namespace ai {
namespace chat {
namespace binders {

template<typename History>
openai<History>::binding::binding(::ai::chat::histories::slot<History> &&s_history)
    : _s_history{ ::std::move(s_history) } {

};

template<typename History>
template<typename Moderator>
openai<History>::binding openai<History>::bind(::ai::chat::histories::observable<History> &history, ::ai::chat::adapters::openai &adapter,
    Moderator &moderator,
    ::std::string_view model, ::std::string_view key,
    size_t skip, ::std::chrono::hours range,
    ::std::string_view pattern, size_t retries, ::std::string_view apology,
    ::std::string_view botname) {
    ::ai::chat::histories::slot<History> s_history{ history.template subscribe<::ai::chat::adapters::openai>() };
    char const USERNAME[]{ "{username}" };
    char const CONTENT[]{ "{content}" };
    s_history.on_message([&history, &adapter,
        &moderator,
        model = ::std::string{ model }, key = ::std::string{ key },
        skip, range,
        pattern = ::std::string{ pattern }, retries, apology = ::std::string{ apology },
            pos_p_username = pattern.find(USERNAME), pos_p_content = pattern.find(CONTENT), pos_a_username = apology.find(USERNAME),
            USERNAME_SIZE = ::std::size(USERNAME), CONTENT_SIZE = ::std::size(CONTENT),
        botname = ::std::string{ botname }
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
        if (username_tag) {
            ::std::string content{ pattern };
            if (!(pos_p_username == ::std::string::npos)) {
                content.replace(pos_p_username, USERNAME_SIZE - 1, username_tag->value);
            }
            if (!(pos_p_content == ::std::string::npos)) {
                content.replace(pos_p_content, CONTENT_SIZE - 1, history_message.content);
            }
            adapter.push_back(::ai::chat::adapters::message{
                ::ai::chat::adapters::role::user,
                content
            });
        } else {
            adapter.push_back(::ai::chat::adapters::message{
                ::ai::chat::adapters::role::system,
                history_message.content
            });
            return;
        }
        ::std::string content{};
        for (size_t left{ retries }; ;) {
            ::ai::chat::adapters::iterator adapter_pos{ adapter.complete(model, key) };
            if (adapter_pos == adapter.end()) {
                ::ai::chat::histories::iterator history_begin{ history.begin() };
                ::ai::chat::histories::iterator history_first{ history_begin + skip };
                ::ai::chat::histories::iterator history_last{ history_first + range };
                ::ai::chat::adapters::iterator adapter_begin{ adapter.begin() };
                ::ai::chat::adapters::iterator adapter_first{ adapter_begin + (history_first - history_begin) };
                ::ai::chat::adapters::iterator adapter_last{ adapter_begin + (history_last - history_begin) };
                adapter.erase(adapter_first, adapter_last);
                history.template erase<::ai::chat::adapters::openai>(history_first, history_last);
                continue;
            }
            ::ai::chat::adapters::message adapter_message{ *adapter_pos };
            if (moderator.is_filtered(adapter_message.content)) {
                if (left--) {
                    adapter.pop_back();
                    continue;
                }
                content = apology;
                if (!(pos_a_username == ::std::string::npos)) {
                    content.replace(pos_a_username, USERNAME_SIZE - 1, username_tag->value);
                }
                break;
            }
            content = adapter_message.content;
            break;
        }
        ::std::vector<::ai::chat::histories::tag> tags{};
        tags.emplace_back("user.name", botname);
        if (channel_tag) {
            tags.emplace_back("channel", channel_tag->value);
        }
        history.template insert<::ai::chat::adapters::openai>(::ai::chat::histories::message{
            {},
            content,
            tags
        });
    });
    s_history.on_erase([&history, &adapter
    ](::ai::chat::histories::iterator history_pos)->void {
        ::ai::chat::histories::iterator history_begin{ history.begin() };
        ::ai::chat::adapters::iterator adapter_begin{ adapter.begin() };
        ::ai::chat::adapters::iterator adapter_pos{ adapter_begin + (history_pos - history_begin) };
        adapter.erase(adapter_pos);
    });
    s_history.on_erase([&history, &adapter
    ](::ai::chat::histories::iterator history_first, ::ai::chat::histories::iterator history_last)->void {
        ::ai::chat::histories::iterator history_begin{ history.begin() };
        ::ai::chat::adapters::iterator adapter_begin{ adapter.begin() };
        ::ai::chat::adapters::iterator adapter_first{ adapter_begin + (history_first - history_begin) };
        ::ai::chat::adapters::iterator adapter_last{ adapter_begin + (history_last - history_begin) };
        adapter.erase(adapter_first, adapter_last);
    });
    return binding{ ::std::move(s_history) };
};

} // binders
} // chat
} // ai

#endif
