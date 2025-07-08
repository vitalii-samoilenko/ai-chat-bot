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
    openai(const openai&) = delete;
    openai(openai&&) = delete;

    ~openai() = delete;

    openai& operator=(const openai&) = delete;
    openai& operator=(openai&&) = delete;

    class binding {
    public:
        binding() = delete;
        binding(const binding&) = delete;
        binding(binding&&) = default;

        ~binding() = default;

        binding& operator=(const binding&) = delete;
        binding& operator=(binding&&) = default;

    private:
        friend openai;

        explicit binding(typename ::ai::chat::histories::observable<History>::slot&& s_history);

        typename ::ai::chat::histories::observable<History>::slot _s_history;
    };

    template<typename Moderator>
    static binding bind(::ai::chat::histories::observable<History>& history, ::ai::chat::adapters::openai& adapter,
        Moderator& moderator,
        const ::std::string& model, const ::std::string& key,
        const ::std::string& pattern, size_t retires, const ::std::string& apology,
        const ::std::string& botname);

};

} // binders
} // chat
} // ai

#include "impl/openai.ipp"

#endif
