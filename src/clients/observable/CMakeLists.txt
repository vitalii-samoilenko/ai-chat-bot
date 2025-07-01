cmake_minimum_required(VERSION 3.25.1)

project(AI.Chat.Clients.Observable VERSION 0.1 LANGUAGES CXX)

add_library(ai_chat_clients_observable INTERFACE)
add_library(AI::Chat::Clients::Observable ALIAS ai_chat_clients_observable)
target_include_directories(ai_chat_clients_observable INTERFACE include)
target_compile_features(ai_chat_clients_observable INTERFACE cxx_std_17)