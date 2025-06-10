#include <exception>
#include <iostream>
#include <vector>

#include "OpenAI/Client.hpp"
#include "Twitch/Auth/Client.hpp"
#include "Twitch/IRC/Client.hpp"

int main() {
    try
    {
        ::OpenAI::Client openAiClient{
            "https://generativelanguage.googleapis.com/v1beta/openai/",
            "apiKey",
            ::std::chrono::milliseconds{ 30 * 1000 }
        };
        ::OpenAI::CompletionContext<::std::vector<::OpenAI::Message>> aiContext{
            "gemini-2.0-flash",
            {
                {
                    ::OpenAI::Role::System,
                    "Message must not be longer than 200 symbols"
                }
            }
        };

        ::Twitch::Auth::Client twitchAuthClient {
            "https://id.twitch.tv/oauth2/",
            ::std::chrono::milliseconds{ 30 * 1000 }
        };
        ::Twitch::Auth::AccessContext accessContext{
            "accessToken",
            "refreshToken"
        };

        ::Twitch::IRC::Client twitchIrcClient {
            "wss://irc-ws.chat.twitch.tv",
            ::std::chrono::milliseconds{ 30 * 1000 }
        };
        ::std::vector<::std::string> channels{ "botname" };

        ::Twitch::IRC::Subscription& openAiSubscription{ twitchIrcClient.Subscribe<::OpenAI::Client>() };
        openAiSubscription.OnMessage([&](::Twitch::IRC::Context& context, const ::Twitch::IRC::Message& message)->void {
            aiContext.Messages.push_back(::OpenAI::Message{ ::OpenAI::Role::User, message.Content });
            ::OpenAI::CompletionResult result{ openAiClient.Complete(aiContext) };
            const ::OpenAI::Message& reply{ result.Choices[0].Message };
            aiContext.Messages.push_back(reply);
            context.Send({ "", message.Channel, reply.Content });
        });

        do {
            if (!twitchAuthClient.ValidateToken(accessContext.AccessToken)) {
                accessContext = twitchAuthClient.RefreshToken(
                    "clientId", "clientSecret",
                    accessContext.RefreshToken);
            }
        }
        while (twitchIrcClient.Run("botname", accessContext.AccessToken, channels));
    }
    catch(const ::std::exception& e)
    {
        ::std::cerr << "Error: " << e.what() << ::std::endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}