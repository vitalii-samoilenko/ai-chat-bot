#ifndef AI_CHAT_COMMANDS_UNMOD_HPP
#define AI_CHAT_COMMANDS_UNMOD_HPP

#include "re2/re2.h"

#include <string_view>

namespace ai {
namespace chat {
namespace commands {

template<typename Moderator>
class unmod {
public:
    static char constexpr name[]{ "unmod" };

    unmod() = delete;
    unmod(unmod const &other) = delete;
    unmod(unmod &&other) = delete;

    ~unmod() = default;

    unmod & operator=(unmod const &other) = delete;
    unmod & operator=(unmod &&other) = delete;

    explicit unmod(Moderator &moderator);

    ::std::string_view execute(::std::string_view args);

private:
    Moderator &_moderator;
    ::RE2 _parser;
};

} // commands
} // chat
} // ai

#include "impl/unmod.ipp"

#endif
