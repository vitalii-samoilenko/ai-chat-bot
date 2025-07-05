cmake_minimum_required(VERSION 3.25.1)

project(AI.Chat.Adapters.OpenAI VERSION 0.1 LANGUAGES CXX)

find_package(OpenSSL REQUIRED)

add_library(ai_chat_adapters_openai INTERFACE)
add_library(AI::Chat::Adapters::OpenAI ALIAS ai_chat_adapters_openai)
target_include_directories(ai_chat_adapters_openai INTERFACE include)
target_link_libraries(ai_chat_adapters_openai
  INTERFACE
    OpenSSL::Crypto
    OpenSSL::SSL
    Boost::url
    EBoost
    opentelemetry-cpp::api
)
target_compile_features(ai_chat_adapters_openai INTERFACE cxx_std_17)
