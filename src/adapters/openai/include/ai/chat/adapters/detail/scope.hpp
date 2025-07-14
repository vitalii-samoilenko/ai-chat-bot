#ifndef AI_CHAT_ADAPTERS_DETAIL_SCOPE_HPP
#define AI_CHAT_ADAPTERS_DETAIL_SCOPE_HPP

#include "boost/json.hpp"

namespace ai {
namespace chat {
namespace adapters {

class iterator;
class openai;

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

    explicit scope(::boost::json::array::const_iterator const &pos);

    ::boost::json::array::const_iterator _pos;
};

} // detail
} // adapters
} // chat
} // ai

#include "impl/scope.ipp"

#endif
