cmake_minimum_required(VERSION 3.25.1)

project(AI.Chat.Clients.Console VERSION 0.1 LANGUAGES CXX)

find_package(re2 REQUIRED)

add_library(ai_chat_clients_console INTERFACE)
target_compile_features(ai_chat_clients_console INTERFACE cxx_std_17)
target_link_libraries(ai_chat_clients_console
  INTERFACE
    AI::Chat::Telemetry
    re2::re2
)
target_include_directories(ai_chat_clients_console INTERFACE include)
add_library(AI::Chat::Clients::Console ALIAS ai_chat_clients_console)
