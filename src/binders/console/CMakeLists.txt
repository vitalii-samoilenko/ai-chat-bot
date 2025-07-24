cmake_minimum_required(VERSION 3.25.1)

project(AI.Chat.Binders.Console VERSION 0.1 LANGUAGES CXX)

add_library(ai_chat_binders_console INTERFACE)
target_compile_features(ai_chat_binders_console INTERFACE cxx_std_20)
target_link_libraries(ai_chat_binders_twitch
  INTERFACE
    AI::Chat::Clients::Observable
    AI::Chat::Clients::Console
    AI::Chat::Commands
    AI::Chat::Histories
    AI::Chat::Moderators
)
target_include_directories(ai_chat_binders_console INTERFACE include)
add_library(AI::Chat::Binders::Console ALIAS ai_chat_binders_console)
