cmake_minimum_required(VERSION 3.25.1)

project(AI.Chat.Commands VERSION 0.1 LANGUAGES CXX)

add_library(ai_chat_commands INTERFACE)
add_library(AI::Chat::Commands ALIAS ai_chat_commands)
target_include_directories(ai_chat_commands INTERFACE include)
target_compile_features(ai_chat_commands INTERFACE cxx_std_17)