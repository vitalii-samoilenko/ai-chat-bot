#include <exception>
#include <iostream>
#include <vector>

#include "OpenAI/Client.hpp"
#include "Twitch/Auth/Client.hpp"

int main() {
    try
    {
        ::OpenAI::Client openAiClient{
            "https://generativelanguage.googleapis.com/v1beta/openai/",
            "apiKey",
            ::std::chrono::seconds{ 30 }
        };
        ::OpenAI::CompletionContext<::std::vector<::OpenAI::Message>> context{
            "gemini-2.0-flash",
            {
                {
                    ::OpenAI::Role::User,
                    "Hello!"
                }
            }
        };
        ::OpenAI::CompletionResult result{ openAiClient.Complete(context) };

        ::std::cout << result.Usage.TotalTokens << ::std::endl;
        ::std::cout << result.Choices[0].Message.Content << ::std::endl;

        ::Twitch::Auth::Client twitchAuthClient {
            "https://id.twitch.tv/oauth2/",
            ::std::chrono::seconds{ 30 }
        };

        bool isValid{ twitchAuthClient.ValidateToken("accessToken") };

        ::std::cout << isValid << ::std::endl;
    }
    catch(const ::std::exception& e)
    {
        ::std::cerr << "Error: " << e.what() << ::std::endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}