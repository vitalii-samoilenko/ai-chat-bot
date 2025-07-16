#ifndef AI_CHAT_COMMANDS_EXECUTOR_HPP
#define AI_CHAT_COMMANDS_EXECUTOR_HPP

#include <string_view>
#include <tuple>

namespace ai {
namespace chat {
namespace commands {

template<typename... Commands>
class executor {
public:
    executor() = delete;
    executor(executor const &other) = delete;
    executor(executor &&other) = delete;

    ~executor() = default;

    executor& operator=(executor const &other) = delete;
    executor& operator=(executor &&other) = delete;

    template<typename... Args>
    explicit executor(Args &&...args);

    template<size_t I = 0>
    ::std::string_view execute(::std::string_view name, ::std::string_view args);

private:
    ::std::tuple<Commands...> _commands;
};

} // commands
} // chat
} // ai

#include "impl/executor.ipp"

#endif
