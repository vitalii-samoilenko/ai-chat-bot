#ifndef AI_CHAT_CLIENTS_AUTH_HPP
#define AI_CHAT_CLIENTS_AUTH_HPP

#include <chrono>
#include <memory>
#include <string>

namespace ai {
namespace chat {
namespace clients {

struct token_context {
    ::std::string access_token;
    ::std::string refresh_token;
};
struct access_context {
    ::std::string device_code;
    ::std::string verification_uri;
};

class auth {
public:
    auth() = delete;
    auth(const auth&) = delete;
    auth(auth&&) = delete;

    ~auth() = default;

    auth& operator=(const auth&) = delete;
    auth& operator=(auth&&) = delete;

    auth(const ::std::string& address, ::std::chrono::milliseconds timeout);

    bool validate_token(const ::std::string& token);
    token_context refresh_token(const ::std::string& client_id, const ::std::string& client_secret, const ::std::string& refresh_token);
    token_context issue_token(const ::std::string& client_id, const ::std::string& device_code, const ::std::string& scopes);
    access_context request_access(const ::std::string& client_id, const ::std::string& scopes);

private:
    class connection;

    ::std::unique_ptr<connection> _service;
};

} // clients
} // chat
} // ai

#include "impl/auth.ipp"

#endif
