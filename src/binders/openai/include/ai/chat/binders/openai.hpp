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
        friend openai;

    public:
        binding() = delete;
        binding(const binding&) = delete;
        binding(binding&&) = default;

        ~binding() = default;

        binding& operator=(const binding&) = delete;
        binding& operator=(binding&&) = default;

    private:
        explicit binding(typename History::slot&& history_slot);

        typename History::slot _history_slot;
    };

    template<typename Moderator>
    static binding bind(History& history, Adapter& adapter,
        Moderator& moderator,
        const ::std::string& botname, const ::std::string& model, const ::std::string& key);

};

} // binders
} // chat
} // ai

#include "impl/openai.ipp"

#endif
