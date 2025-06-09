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

        // ::Twitch::Auth::AccessContext accessContext{
        //     twitchAuthClient.RefreshToken(
        //         "clientId", "clientSecret",
        //         "refreshToken")
        // };

        // ::std::cout << accessContext.AccessToken << ::std::endl;

        ::Twitch::IRC::Client twitchIrcClient {
            "wss://irc-ws.chat.twitch.tv",
            ::std::chrono::milliseconds{ 30 * 1000 }
        };
        ::Twitch::IRC::Subscription& openAiSubscription{ twitchIrcClient.Subscribe<::OpenAI::Client>() };
        openAiSubscription.OnMessage([&](::Twitch::IRC::Context& context, const ::Twitch::IRC::Message& message)->void {
            aiContext.Messages.push_back(::OpenAI::Message{ ::OpenAI::Role::User, message.Content });
            ::OpenAI::CompletionResult result{ openAiClient.Complete(aiContext) };
            const ::OpenAI::Message& reply{ result.Choices[0].Message };
            aiContext.Messages.push_back(reply);
            ::std::cout << result.Usage.TotalTokens << ::std::endl;

            context.Send({
                "", "channel",
                reply.Content
            });
            //context.Disconnect();
        });
        
        while (twitchIrcClient.Run(
            "channel", "accessToken",
            ::std::vector<::std::string>{
                "channel"
            }));
    }
    catch(const ::std::exception& e)
    {
        ::std::cerr << "Error: " << e.what() << ::std::endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}