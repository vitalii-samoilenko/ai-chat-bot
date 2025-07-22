cmake_minimum_required(VERSION 3.25.1)

project(AI.Chat.Adapters.OpenAI VERSION 0.1 LANGUAGES CXX)

find_package(Boost CONFIG REQUIRED COMPONENTS asio beast scope)
find_package(EBoost CONFIG REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(re2 REQUIRED)

add_library(ai_chat_adapters_openai INTERFACE)
target_compile_features(ai_chat_adapters_openai INTERFACE cxx_std_17)
target_link_libraries(ai_chat_adapters_openai
  INTERFACE
    AI::Chat::Telemetry
    Boost::asio
    Boost::beast
    Boost::scope
    EBoost::eboost
    OpenSSL::Crypto
    OpenSSL::SSL
    re2::re2
)
target_include_directories(ai_chat_adapters_openai INTERFACE include)
add_library(AI::Chat::Adapters::OpenAI ALIAS ai_chat_adapters_openai)
