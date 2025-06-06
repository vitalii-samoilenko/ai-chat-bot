#include <exception>
#include <iostream>
#include <vector>

#include "OpenAI/Client.hpp"

int main() {
    try
    {
        ::OpenAI::Client client{ "https://generativelanguage.googleapis.com/v1beta/openai/", "apiKey" };
        ::std::vector<::OpenAI::Message> messages{
            {
                ::OpenAI::Role::User,
                "Explain to me how AI works"
            }
        };
        ::OpenAI::Message answer{ client.Complete("gemini-2.0-flash", messages.begin(), messages.end()) };

        ::std::cout << answer.Content << ::std::endl;
    }
    catch(const ::std::exception& e)
    {
        ::std::cerr << "Error: " << e.what() << ::std::endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}