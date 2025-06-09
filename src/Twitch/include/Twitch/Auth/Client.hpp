#ifndef TWITCH_AUTH_CLIENT_HPP
#define TWITCH_AUTH_CLIENT_HPP

#include <chrono>
#include <string>

namespace Twitch {
namespace Auth {

struct AccessContext {
    ::std::string AccessToken;
    ::std::string RefreshToken;
};
struct AuthContext {
    ::std::string DeviceCode;
    ::std::string VerificationUri;
};

class Client {
public:
    Client() = delete;
    Client(const Client&) = delete;
    Client(Client&&) = delete;

    ~Client() = default;

    Client& operator=(const Client&) = delete;
    Client& operator=(Client&&) = delete;

    Client(const ::std::string& baseAddress, ::std::chrono::milliseconds timeout);

    bool ValidateToken(const ::std::string& token) const;
    AccessContext RefreshToken(const ::std::string& clientId, const ::std::string& clientSecret, const ::std::string& refreshToken) const;
    AccessContext IssueToken(const ::std::string& clientId, const ::std::string& deviceCode, const ::std::string& scopes) const;
    AuthContext RequestAccess(const ::std::string& clientId, const ::std::string& scopes) const;

private:
    bool m_ssl;
    ::std::string m_host;
    ::std::string m_port;
    ::std::string m_validateTarget;
    ::std::string m_tokenTarget;
    ::std::string m_deviceTarget;
    ::std::chrono::milliseconds m_timeout;
};

} // Auth
} // Twitch

#include "impl/Client.ipp"

#endif
