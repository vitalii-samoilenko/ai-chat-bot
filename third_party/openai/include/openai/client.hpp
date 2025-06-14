#ifndef OPENAI_CLIENT_HPP
#define OPENAI_CLIENT_HPP

#include <chrono>
#include <string>
#include <vector>

namespace openai {

enum class role {
    system,
    user,
    assistant
};
struct message {
    ::openai::role role;
    ::std::string content;
};
struct usage {
    size_t completion_tokens;
    size_t prompt_tokens;
    size_t total_tokens;
};
enum class finish_reason {
    stop,
    length,
    content_filter,
    tool_calls
};
struct choice {
    ::openai::finish_reason finis_reason;
    size_t index;
    ::openai::message message;
};
struct completion_result {
    ::std::vector<::openai::choice> choices;
    ::openai::usage usage;
};
template<typename Range>
struct completion_context {
    ::std::string model;
    Range messages;
};

class client {
public:
    client() = delete;
    client(const client&) = delete;
    client(client&&) = delete;

    ~client() = default;

    client& operator=(const client&) = delete;
    client& operator=(client&&) = delete;

    client(const ::std::string& address, const ::std::string& key, ::std::chrono::milliseconds timeout);

    template<typename Range>
    ::openai::completion_result complete(const ::openai::completion_context<Range>& context) const;

private:
    bool _ssl;
    ::std::string _host;
    ::std::string _port;
    ::std::string _target_completions;
    ::std::string _authorization;
    ::std::chrono::milliseconds _timeout;
};

} // openai

#include "impl/client.ipp"

#endif
