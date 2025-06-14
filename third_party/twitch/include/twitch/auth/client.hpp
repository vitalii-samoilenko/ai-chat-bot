#ifndef TWITCH_AUTH_CLIENT_HPP
#define TWITCH_AUTH_CLIENT_HPP

#include <chrono>
#include <string>

namespace twitch {
namespace auth {

struct access_context {
    ::std::string access_token;
    ::std::string refresh_token;
};
struct auth_context {
    ::std::string device_code;
    ::std::string verification_uri;
};

class client {
public:
    client() = delete;
    client(const client&) = delete;
    client(client&&) = delete;

    ~client() = default;

    client& operator=(const client&) = delete;
    client& operator=(client&&) = delete;

    client(const ::std::string& address, ::std::chrono::milliseconds timeout);

    bool validate_token(const ::std::string& token) const;
    access_context refresh_token(const ::std::string& client_id, const ::std::string& client_secret, const ::std::string& refresh_token) const;
    access_context issue_token(const ::std::string& client_id, const ::std::string& device_code, const ::std::string& scopes) const;
    auth_context request_access(const ::std::string& client_id, const ::std::string& scopes) const;

private:
    bool _ssl;
    ::std::string _host;
    ::std::string _port;
    ::std::string _target_validate;
    ::std::string _target_token;
    ::std::string _target_device;
    ::std::chrono::milliseconds _timeout;
};

} // auth
} // twitch

#include "impl/client.ipp"

#endif
