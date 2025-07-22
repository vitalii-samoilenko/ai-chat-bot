cmake_minimum_required(VERSION 3.25.1)

project(AI.Chat.Clients.Twitch VERSION 0.1 LANGUAGES CXX)

find_package(Boost CONFIG REQUIRED COMPONENTS asio beast)
find_package(EBoost CONFIG REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(opentelemetry-cpp REQUIRED)
find_package(re2 REQUIRED)

add_library(ai_chat_clients_twitch INTERFACE)
target_compile_features(ai_chat_clients_twitch INTERFACE cxx_std_17)
target_link_libraries(ai_chat_clients_twitch
  INTERFACE
    AI::Chat::Telemetry
    Boost::asio
    Boost::beast
    EBoost::eboost
    OpenSSL::Crypto
    OpenSSL::SSL
    opentelemetry-cpp::api
    re2::re2
)
target_include_directories(ai_chat_clients_twitch INTERFACE include)
add_library(AI::Chat::Clients::Twitch ALIAS ai_chat_clients_twitch)
