#ifndef AI_CHAT_BINDERS_OPENAI_HPP
#define AI_CHAT_BINDERS_OPENAI_HPP

#include "ai/chat/adapters/openai.hpp"
#include "ai/chat/histories/observable.hpp"

namespace ai {
namespace chat {
namespace binders {

template<typename History>
class openai {
public:
    openai() = delete;
    openai(openai const &other) = delete;
    openai(openai &&other) = delete;

    ~openai() = delete;

    openai & operator=(openai const &other) = delete;
    openai & operator=(openai &&other) = delete;

    class binding {
    public:
        binding() = delete;
        binding(binding const &other) = delete;
        binding(binding &&other) = default;

        ~binding() = default;

        binding & operator=(binding const &other) = delete;
        binding & operator=(binding &&other) = delete;

    private:
        friend openai;

        explicit binding(::ai::chat::histories::slot<History> &&s_history);

        ::ai::chat::histories::slot<History> _s_history;
    };

    template<typename Moderator, typename ContentFormatter, typename ApologyFormatter>
    static binding bind(::ai::chat::histories::observable<History> &history, ::ai::chat::adapters::openai &adapter,
        Moderator &moderator,
        ::std::string_view model, ::std::string_view key,
        size_t skip, ::std::chrono::hours range,
        ContentFormatter &f_content, size_t retries, ApologyFormatter &f_apology,
        ::std::string_view botname);
};

} // binders
} // chat
} // ai

#include "impl/openai.ipp"

#endif
