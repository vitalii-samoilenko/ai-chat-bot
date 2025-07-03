#ifndef AI_CHAT_COMMANDS_JOIN_HPP
#define AI_CHAT_COMMANDS_JOIN_HPP

#include <string>

namespace ai {
namespace chat {
namespace commands {

template<typename Client>
class join {
public:
    static constexpr char name[]{ "join" };

    join() = delete;
    join(const join&) = delete;
    join(join&&) = delete;

    ~join() = default;

    join& operator=(const join&) = delete;
    join& operator=(join&&) = delete;

    explicit join(Client& client);

    ::std::string execute(const ::std::string& args);

private:
    Client& _client;
};

} // commands
} // chat
} // ai

#include "impl/join.ipp"

#endif
