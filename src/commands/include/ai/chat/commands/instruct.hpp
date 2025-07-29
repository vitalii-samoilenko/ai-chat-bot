#ifndef AI_CHAT_COMMANDS_INSTRUCT_HPP
#define AI_CHAT_COMMANDS_INSTRUCT_HPP

#include <string_view>
#include <vector>

#include "ai/chat/histories/observable.hpp"

namespace ai {
namespace chat {
namespace commands {

template<typename History>
class instruct {
public:
    static char constexpr name[]{ "instruct" };

    instruct() = delete;
    instruct(instruct const &other) = delete;
    instruct(instruct &&other) = delete;

    ~instruct() = default;

    instruct & operator=(instruct const &other) = delete;
    instruct & operator=(instruct &&other) = delete;

    explicit instruct(::ai::chat::histories::observable<History> &history);

    ::std::string_view execute(::std::string_view args);

private:
    ::ai::chat::histories::observable<History> &_history;
    ::std::vector<::ai::chat::histories::tag> _tags;
};

} // commands
} // chat
} // ai

#include "impl/instruct.ipp"

#endif
