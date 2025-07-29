#ifndef AI_CHAT_ADAPTERS_DETAIL_SCOPE_HPP
#define AI_CHAT_ADAPTERS_DETAIL_SCOPE_HPP

#include "boost/json.hpp"

#include "ai/chat/adapters/openai.hpp"

namespace ai {
namespace chat {
namespace adapters {
namespace detail {

class scope {
private:
    friend iterator;
    friend openai;

    scope() = delete;
    scope(scope const &other) = delete;
    scope(scope &&other) = delete;

    ~scope() = default;

    scope & operator=(scope const &other) = delete;
    scope & operator=(scope &&other) = delete;

    explicit scope(::boost::json::array::iterator const &pos);

    ::boost::json::array::iterator _pos;
};

} // detail
} // adapters
} // chat
} // ai

#include "impl/scope.ipp"

#endif
