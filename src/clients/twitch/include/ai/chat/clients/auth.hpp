#ifndef AI_CHAT_CLIENTS_AUTH_HPP
#define AI_CHAT_CLIENTS_AUTH_HPP

#include <chrono>
#include <string_view>

namespace ai {
namespace chat {
namespace clients {

struct token_context {
    ::std::string_view access_token;
    ::std::string_view refresh_token;
};
struct access_context {
    ::std::string_view device_code;
    ::std::string_view verification_uri;
};

class auth;

} // clients
} // chat
} // ai

#include "detail/auth_connection.hpp"

namespace ai {
namespace chat {
namespace clients {

class auth {
public:
    auth() = delete;
    auth(auth const &) = delete;
    auth(auth &&) = delete;

    ~auth() = default;

    auth & operator=(auth const &) = delete;
    auth & operator=(auth &&) = delete;

    auth(::std::string_view address, ::std::chrono::milliseconds timeout);

    bool validate_token(::std::string_view token);
    token_context refresh_token(::std::string_view client_id, ::std::string_view client_secret, ::std::string_view refresh_token);
    token_context issue_token(::std::string_view client_id, ::std::string_view device_code, ::std::string_view scopes);
    access_context request_access(::std::string_view client_id, ::std::string_view scopes);

private:
    detail::auth_connection _service;
};

} // clients
} // chat
} // ai

#include "impl/auth.ipp"

#endif
