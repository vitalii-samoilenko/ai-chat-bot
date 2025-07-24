#ifndef AI_CHAT_BINDERS_CONSOLE_HPP
#define AI_CHAT_BINDERS_CONSOLE_HPP

#include "ai/chat/clients/console.hpp"
#include "ai/chat/clients/observable.hpp"
#include "ai/chat/commands/executor.hpp"
#include "ai/chat/histories/observable.hpp"

namespace ai {
namespace chat {
namespace binders {

template<typename History>
class console {
public:
    console() = delete;
    console(console const &other) = delete;
    console(console &&other) = delete;

    ~console() = delete;

    console & operator=(console const &other) = delete;
    console & operator=(console &&other) = delete;

    class binding {
    public:
        binding() = delete;
        binding(binding const &other) = delete;
        binding(binding &&other) = default;

        ~binding() = default;

        binding & operator=(binding const &other) = delete;
        binding & operator=(binding &&other) = default;

    private:
        friend console;

        binding(::ai::chat::histories::slot<History> &&s_history, ::ai::chat::clients::slot<::ai::chat::clients::console> &&s_client);

        ::ai::chat::histories::slot<History> _s_history;
        ::ai::chat::clients::slot<::ai::chat::clients::console> _s_client;
    };

    template<typename Moderator, typename... Commands>
    static binding bind(::ai::chat::histories::observable<History> &history, ::ai::chat::clients::observable<::ai::chat::clients::console> &client,
        Moderator &moderator, ::ai::chat::commands::executor<Commands...> &executor,
        ::std::string_view botname);
};

} // binders
} // chat
} // ai

#include "impl/console.ipp"

#endif
