#ifndef AI_CHAT_BINDERS_OPENAI_HPP
#define AI_CHAT_BINDERS_OPENAI_HPP

#include "ai/chat/adapters/openai.hpp"

namespace ai {
namespace chat {
namespace binders {

template<typename History, typename Adapter>
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

        explicit binding(typename History::slot&& s_history);

        typename History::slot _s_history;
    };

    template<typename Moderator>
    static binding bind(History& history, Adapter& adapter,
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
