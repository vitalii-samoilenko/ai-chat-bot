#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <string>

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

class Client {
public:
    Client() = delete;
    Client(const Client&) = delete;
    Client(Client&&) = delete;

    ~Client() = default;

    Client& operator=(const Client&) = delete;
    Client& operator=(Client&&) = delete;

    Client(const ::std::string& baseAddress, const ::std::string& apiKey);

    template<typename Iterator>
    Message Complete(const ::std::string& model, Iterator begin, Iterator end);

private:
    ::std::string m_host;
    ::std::string m_port;
    ::std::string m_target;
    bool m_ssl;
    ::std::string m_authorization;
};

} // OpenAI

#include "impl/Client.ipp"

#endif
