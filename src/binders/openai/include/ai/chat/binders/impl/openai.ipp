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
template<typename Moderator, typename ContentFormatter, typename ApologyFormatter>
openai<History>::binding openai<History>::bind(::ai::chat::histories::observable<History> &history, ::ai::chat::adapters::openai &adapter,
    Moderator &moderator,
    ::std::string_view model, ::std::string_view key,
    size_t skip, ::std::chrono::hours range,
    ContentFormatter &f_content, size_t retries, ApologyFormatter &f_apology,
    ::std::string_view botname) {
    ::ai::chat::histories::slot<History> s_history{ history.template subscribe<::ai::chat::adapters::openai>() };
    s_history.on_message([&history, &adapter,
        &moderator,
        model = ::std::string{ model }, key = ::std::string{ key },
        skip, range,
        &f_content, retries, &f_apology,
        botname = ::std::string{ botname }
    ](::ai::chat::histories::observable_iterator<History> history_pos)->void {
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
            adapter.push_back(::ai::chat::adapters::message{
                ::ai::chat::adapters::role::user,
                f_content(username_tag->value, history_message.content)
            });
        } else {
            adapter.push_back(::ai::chat::adapters::message{
                ::ai::chat::adapters::role::system,
                history_message.content
            });
            return;
        }
        for (size_t left{ retries }; ;) {
            try {
                adapter.complete(model, key);
            }
            catch (::std::overflow_error const &e) {
                ::ai::chat::histories::observable_iterator<History> history_begin{ history.begin() };
                ::ai::chat::histories::observable_iterator<History> history_first{ history_begin + skip };
                ::ai::chat::histories::observable_iterator<History> history_last{ history_first + range };
                ::ai::chat::adapters::iterator adapter_begin{ adapter.begin() };
                ::ai::chat::adapters::iterator adapter_first{ adapter_begin + (history_first - history_begin) };
                ::ai::chat::adapters::iterator adapter_last{ adapter_begin + (history_last - history_begin) };
                adapter.erase(adapter_first, adapter_last);
                history.template erase<::ai::chat::adapters::openai>(history_first, history_last);
                continue;
            }
            catch (::std::exception const &e) {
                adapter.push_back(::ai::chat::adapters::message{
                    ::ai::chat::adapters::role::assistant,
                    f_apology(username_tag->value)
                });
                break;
            }
            ::ai::chat::adapters::message adapter_message{ adapter.back() };
            if (!moderator.is_filtered(adapter_message.content)) {
                break;
            }
            adapter.pop_back();
            if (left--) {
                continue;
            }
            adapter.push_back(::ai::chat::adapters::message{
                ::ai::chat::adapters::role::assistant,
                f_apology(username_tag->value)
            });
            break;
        }
        ::std::vector<::ai::chat::histories::tag> tags{};
        tags.emplace_back("source", "openai");
        tags.emplace_back("user.name", botname);
        if (channel_tag) {
            tags.emplace_back("channel", channel_tag->value);
        }
        ::ai::chat::adapters::message adapter_message{ adapter.back() };
        history.template insert<::ai::chat::adapters::openai>(::ai::chat::histories::message{
            ::std::chrono::nanoseconds{},
            adapter_message.content,
            tags
        });
    });
    s_history.on_erase([&history, &adapter
    ](::ai::chat::histories::observable_iterator<History> history_pos)->void {
        ::ai::chat::histories::observable_iterator<History> history_begin{ history.begin() };
        ::ai::chat::adapters::iterator adapter_begin{ adapter.begin() };
        ::ai::chat::adapters::iterator adapter_pos{ adapter_begin + (history_pos - history_begin) };
        adapter.erase(adapter_pos);
    });
    s_history.on_erase([&history, &adapter
    ](::ai::chat::histories::observable_iterator<History> history_first, ::ai::chat::histories::observable_iterator<History> history_last)->void {
        ::ai::chat::histories::observable_iterator<History> history_begin{ history.begin() };
        ::ai::chat::adapters::iterator adapter_begin{ adapter.begin() };
        ::ai::chat::adapters::iterator adapter_first{ adapter_begin + (history_first - history_begin) };
        ::ai::chat::adapters::iterator adapter_last{ adapter_begin + (history_last - history_begin) };
        adapter.erase(adapter_first, adapter_last);
    });
    s_history.on_update([&history, &adapter,
        &f_content,
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
        ::ai::chat::histories::observable_iterator<History> history_begin{ history.begin() };
        ::ai::chat::adapters::iterator adapter_begin{ adapter.begin() };
        ::ai::chat::adapters::iterator adapter_pos{ adapter_begin + (history_pos - history_begin) };
        adapter_pos = username_tag && !(username_tag->value == botname)
            ? f_content(username_tag->value, history_message.content)
            : history_message.content;
    });
    return binding{ ::std::move(s_history) };
};

} // binders
} // chat
} // ai

#endif
