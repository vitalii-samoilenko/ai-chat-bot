#ifndef AI_CHAT_COMMANDS_EDIT_HPP
#define AI_CHAT_COMMANDS_EDIT_HPP

#include <string_view>

#include "re2/re2.h"

#include "ai/chat/histories/observable.hpp"

namespace ai {
namespace chat {
namespace commands {

template<typename History>
class edit {
public:
    static char constexpr name[]{ "edit" };

    edit() = delete;
    edit(edit const &other) = delete;
    edit(edit &&other) = delete;

    ~edit() = default;

    edit & operator=(edit const &other) = delete;
    edit & operator=(edit &&other) = delete;

    explicit edit(::ai::chat::histories::observable<History> &history);

    ::std::string_view execute(::std::string_view args);

private:
    ::ai::chat::histories::observable<History> &_history;
    ::RE2 _parser;
};

} // commands
} // chat
} // ai

#include "impl/edit.ipp"

#endif
