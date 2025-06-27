cmake_minimum_required(VERSION 3.25.1)

project(AI.Chat.Binders.Twitch VERSION 0.1 LANGUAGES CXX)


add_library(ai_chat_binders_twitch INTERFACE)
add_library(AI::Chat::Binders::Twitch ALIAS ai_chat_binders_twitch)
target_include_directories(ai_chat_binders_twitch INTERFACE include)
target_link_libraries(ai_chat_binders_twitch
  INTERFACE
    AI::Chat::Clients::Twitch
    AI::Chat::Histories::Observable
)
target_compile_features(ai_chat_binders_twitch INTERFACE cxx_std_11)
