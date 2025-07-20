#ifndef AI_CHAT_COMMANDS_ALLOW_HPP
#define AI_CHAT_COMMANDS_ALLOW_HPP

#include <string_view>

#include "re2/re2.h"

namespace ai {
namespace chat {
namespace commands {

template<typename Moderator>
class allow {
public:
    static char constexpr name[]{ "allow" };

    allow() = delete;
    allow(allow const &other) = delete;
    allow(allow &&other) = delete;

    ~allow() = default;

    allow & operator=(allow const &other) = delete;
    allow & operator=(allow &&other) = delete;

    explicit allow(Moderator &moderator);

    ::std::string_view execute(::std::string_view args);

private:
    Moderator &_moderator;
    ::RE2 _parser;
};

} // commands
} // chat
} // ai

#include "impl/allow.ipp"

#endif
