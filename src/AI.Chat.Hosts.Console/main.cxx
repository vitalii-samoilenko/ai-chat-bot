#include <exception>
#include <iostream>
#include <vector>

#include "OpenAI/Client.hpp"

int main() {
    try
    {
        ::OpenAI::Client client{ "https://generativelanguage.googleapis.com/v1beta/openai/", "apiKey" };
        ::OpenAI::CompletionContext<::std::vector<::OpenAI::Message>> context{
            "gemini-2.0-flash",
            {
                {
                    ::OpenAI::Role::User,
                    "Explain to me how AI works"
                }
            }
        };
        ::OpenAI::CompletionResult result{ client.Complete(context) };

        ::std::cout << result.Usage.TotalTokens << ::std::endl;
        ::std::cout << result.Choices[0].Message.Content << ::std::endl;
    }
    catch(const ::std::exception& e)
    {
        ::std::cerr << "Error: " << e.what() << ::std::endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}