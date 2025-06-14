cmake_minimum_required(VERSION 3.25.1)

project(AI.Chat.Adapters.Common VERSION 0.1 LANGUAGES CXX)

add_library(ai_chat_adapters_common INTERFACE)
add_library(AI::Chat::Adapters::Common ALIAS ai_chat_adapters_common)
target_include_directories(ai_chat_adapters_common INTERFACE include)
target_compile_features(ai_chat_adapters_common INTERFACE cxx_std_11)