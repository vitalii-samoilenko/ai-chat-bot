#ifndef AI_CHAT_COMMANDS_EXECUTOR_HPP
#define AI_CHAT_COMMANDS_EXECUTOR_HPP

#include <string>
#include <tuple>

namespace ai {
namespace chat {
namespace commands {

template<typename... Commands>
class executor {
public:
    executor() = delete;
    executor(const executor&) = delete;
    executor(executor&&) = delete;

    ~executor() = default;

    executor& operator=(const executor&) = delete;
    executor& operator=(executor&&) = delete;

    template<typename... Args>
    explicit executor(Args&& ...args);

    template<size_t I = 0>
    ::std::string execute(const ::std::string& name, const ::std::string& args);

private:
    ::std::tuple<Commands...> _commands;
};

} // commands
} // chat
} // ai

#include "impl/executor.ipp"

#endif
