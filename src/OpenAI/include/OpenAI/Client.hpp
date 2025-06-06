#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <string>
#include <vector>

namespace OpenAI {

enum class Role {
    System,
    User,
    Assistant
};
struct Message {
    ::OpenAI::Role Role;
    ::std::string Content;
};
struct Usage {
    size_t CompletionTokens;
    size_t PromptTokens;
    size_t TotalTokens;
};
enum class FinishReason {
    Stop,
    Length,
    ContentFilter,
    ToolCalls
};
struct Choice {
    ::OpenAI::FinishReason FinishReason;
    size_t Index;
    ::OpenAI::Message Message;
};
struct CompletionResult {
    ::std::vector<::OpenAI::Choice> Choices;
    ::OpenAI::Usage Usage;
};
template<typename Range>
struct CompletionContext {
    ::std::string Model;
    Range Messages;
};

class Client {
public:
    Client() = delete;
    Client(const Client&) = delete;
    Client(Client&&) = delete;

    ~Client() = default;

    Client& operator=(const Client&) = delete;
    Client& operator=(Client&&) = delete;

    Client(const ::std::string& baseAddress, const ::std::string& apiKey);

    template<typename Range>
    ::OpenAI::CompletionResult Complete(const ::OpenAI::CompletionContext<Range>& context);

private:
    bool m_ssl;
    ::std::string m_host;
    ::std::string m_port;
    ::std::string m_completionsPath;
    ::std::string m_authorization;
};

} // OpenAI

#include "impl/Client.ipp"

#endif
