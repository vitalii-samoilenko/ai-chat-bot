#ifndef AI_CHAT_COMMANDS_EXECUTOR_IPP
#define AI_CHAT_COMMANDS_EXECUTOR_IPP

#include <utility>

#include "ai/chat/commands/executor.hpp"

namespace ai {
namespace chat {
namespace commands {

template<typename... Commands>
template<typename... Args>
executor<Commands...>::executor(Args&& ...args)
    : _commands{ ::std::forward<Args>(args)... } {

};

template<typename... Commands>
template<size_t I>
::std::string executor<Commands...>::execute(const ::std::string& name, const ::std::string& args) {
    if constexpr (!(I < ::std::tuple_size_v<decltype(_commands)>)) {
        return {};
    } else {
        if (name == ::std::tuple_element_t<I, decltype(_commands)>::name) {
            return ::std::get<I>(_commands).execute(args);
        }
        return execute<I + 1>(name, args);
    }
};

} // commands
} // chat
} // ai

#endif
