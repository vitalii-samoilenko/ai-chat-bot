#ifndef AI_CHAT_ADAPTERS_CONTENT_LENGTH_HPP
#define AI_CHAT_ADAPTERS_CONTENT_LENGTH_HPP

#include <string>
#include <utility>

#include "opentelemetry/metrics/sync_instruments.h"
#include "opentelemetry/nostd/unique_ptr.h"

namespace ai {
namespace chat {
namespace adapters {

template<typename Adapter>
class content_length : private Adapter {
public:
    content_length() = delete;
    content_length(const content_length&) = delete;
    content_length(content_length&&) = delete;

    ~content_length() = default;

    content_length& operator=(const content_length&) = delete;
    content_length& operator=(content_length&&) = delete;

    template<typename... Args>
    explicit content_length(Args&& ...args);

    ::std::pair<::std::string, size_t> complete() const;

private:
    ::opentelemetry::nostd::unique_ptr<::opentelemetry::metrics::Histogram<uint64_t>> _p_content_length;
};

} // adapters
} // chat
} // ai

#include "impl/content_length.ipp"

#endif
