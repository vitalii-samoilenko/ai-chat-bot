#ifndef AI_CHAT_ADAPTERS_TRACE_HPP
#define AI_CHAT_ADAPTERS_TRACE_HPP

#include <string>
#include <utility>

#include "opentelemetry/trace/tracer.h"
#include "opentelemetry/nostd/shared_ptr.h"

namespace ai {
namespace chat {
namespace adapters {

template<typename Adapter>
class trace : private Adapter {
public:
    trace() = delete;
    trace(const trace&) = delete;
    trace(trace&&) = delete;

    ~trace() = default;

    trace& operator=(const trace&) = delete;
    trace& operator=(trace&&) = delete;

    template<typename... Args>
    explicit trace(Args&& ...args);

    ::std::pair<::std::string, size_t> complete() const;

private:
    ::opentelemetry::nostd::shared_ptr<::opentelemetry::trace::Tracer> _p_tracer;
};

} // adapters
} // chat
} // ai

#include "impl/trace.ipp"

#endif
