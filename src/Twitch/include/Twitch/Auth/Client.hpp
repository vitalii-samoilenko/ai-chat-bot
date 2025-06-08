#ifndef TWITCH_AUTH_CLIENT_HPP
#define TWITCH_AUTH_CLIENT_HPP

#include <chrono>
#include <string>

namespace Twitch {
namespace Auth {

class Client {
public:
    Client() = delete;
    Client(const Client&) = delete;
    Client(Client&&) = delete;

    ~Client() = default;

    Client& operator=(const Client&) = delete;
    Client& operator=(Client&&) = delete;

    Client(const ::std::string& baseAddress, ::std::chrono::steady_clock::duration timeout);

    bool ValidateToken(const ::std::string& token);
    ::std::string RefreshToken(const ::std::string& clientId, const ::std::string& clientSecret, const ::std::string& refreshToken);

private:
    bool m_ssl;
    ::std::string m_host;
    ::std::string m_port;
    ::std::string m_validateTarget;
    ::std::string m_tokenTarget;
    ::std::chrono::steady_clock::duration m_timeout;
};

} // Auth
} // Twitch

#include "impl/Client.ipp"

#endif
