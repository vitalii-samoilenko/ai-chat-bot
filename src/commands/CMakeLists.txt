cmake_minimum_required(VERSION 3.25.1)

project(AI.Chat.Commands VERSION 0.1 LANGUAGES CXX)

find_package(re2 REQUIRED)

add_library(ai_chat_commands INTERFACE)
target_compile_features(ai_chat_commands INTERFACE cxx_std_17)
target_link_libraries(ai_chat_commands
  INTERFACE
    AI::Chat::Histories
    AI::Chat::Moderators
    re2::re2
)
target_include_directories(ai_chat_commands INTERFACE include)
add_library(AI::Chat::Commands ALIAS ai_chat_commands)
