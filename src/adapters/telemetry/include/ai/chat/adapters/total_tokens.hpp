#ifndef AI_CHAT_ADAPTERS_TOTAL_TOKENS_HPP
#define AI_CHAT_ADAPTERS_TOTAL_TOKENS_HPP

#include <string>
#include <utility>

#include "opentelemetry/metrics/sync_instruments.h"
#include "opentelemetry/nostd/unique_ptr.h"

namespace ai {
namespace chat {
namespace adapters {

template<typename Adapter>
class total_tokens {
public:
    total_tokens() = delete;
    total_tokens(const total_tokens&) = delete;
    total_tokens(total_tokens&&) = delete;

    ~total_tokens() = default;

    total_tokens& operator=(const total_tokens&) = delete;
    total_tokens& operator=(total_tokens&&) = delete;

    template<typename... Args>
    explicit total_tokens(Args&& ...args);

    Adapter& next();

    ::std::pair<::std::string, size_t> complete() const;

private:
    Adapter _next;
    ::opentelemetry::nostd::unique_ptr<::opentelemetry::metrics::Gauge<int64_t>> _p_total_tokens;
};

} // adapters
} // chat
} // ai

#include "impl/total_tokens.ipp"

#endif
