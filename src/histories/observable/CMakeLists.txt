cmake_minimum_required(VERSION 3.25.1)

project(AI.Chat.Histories.Observable VERSION 0.1 LANGUAGES CXX)

add_library(ai_chat_histories_observable INTERFACE)
target_compile_features(ai_chat_histories_observable INTERFACE cxx_std_11)
target_include_directories(ai_chat_histories_observable INTERFACE include)
add_library(AI::Chat::Histories::Observable ALIAS ai_chat_histories_observable)
