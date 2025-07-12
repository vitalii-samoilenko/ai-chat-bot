cmake_minimum_required(VERSION 3.25.1)

project(AI.Chat.Binders.OpenAI VERSION 0.1 LANGUAGES CXX)

add_library(ai_chat_binders_openai INTERFACE)
target_compile_features(ai_chat_binders_openai INTERFACE cxx_std_11)
target_link_libraries(ai_chat_binders_openai
  INTERFACE
    AI::Chat::Adapters::OpenAI
    AI::Chat::Histories
    AI::Chat::Moderators
)
target_include_directories(ai_chat_binders_openai INTERFACE include)
add_library(AI::Chat::Binders::OpenAI ALIAS ai_chat_binders_openai)
