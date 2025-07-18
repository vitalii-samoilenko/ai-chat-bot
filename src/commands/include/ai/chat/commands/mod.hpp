#ifndef AI_CHAT_COMMANDS_MOD_HPP
#define AI_CHAT_COMMANDS_MOD_HPP

#include "re2/re2.h"

#include <string_view>

namespace ai {
namespace chat {
namespace commands {

template<typename Moderator>
class mod {
public:
    static char constexpr name[]{ "mod" };

    mod() = delete;
    mod(mod const &other) = delete;
    mod(mod &&other) = delete;

    ~mod() = default;

    mod & operator=(mod const &other) = delete;
    mod & operator=(mod &&other) = delete;

    explicit mod(Moderator &moderator);

    ::std::string_view execute(::std::string_view args);

private:
    Moderator &_moderator;
    ::RE2 _parser;
};

} // commands
} // chat
} // ai

#include "impl/mod.ipp"

#endif
