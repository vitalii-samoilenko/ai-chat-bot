cmake_minimum_required(VERSION 3.25.1)

project(AI.Chat.Clients.Twitch VERSION 0.1 LANGUAGES CXX)

find_package(OpenSSL REQUIRED)
find_package(Boost CONFIG REQUIRED COMPONENTS url xpressive)
find_package(opentelemetry-cpp REQUIRED)

add_library(ai_chat_clients_twitch INTERFACE)
add_library(AI::Chat::Clients::Twitch ALIAS ai_chat_clients_twitch)
target_include_directories(ai_chat_clients_twitch INTERFACE include)
target_link_libraries(ai_chat_clients_twitch
  INTERFACE
    OpenSSL::Crypto
    OpenSSL::SSL
    Boost::url
    Boost::xpressive
    EBoost
    opentelemetry-cpp::api
)
target_compile_features(ai_chat_clients_twitch INTERFACE cxx_std_17)