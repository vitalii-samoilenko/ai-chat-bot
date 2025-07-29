#ifndef AI_CHAT_ADAPTERS_DETAIL_SCOPE_IPP
#define AI_CHAT_ADAPTERS_DETAIL_SCOPE_IPP

#include "ai/chat/adapters/detail/scope.hpp"

namespace ai {
namespace chat {
namespace adapters {
namespace detail {

scope::scope(::boost::json::array::iterator const &pos)
    : _pos{ pos } {

};

} // detail
} // adapters
} // chat
} // ai

#endif
