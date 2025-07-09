cmake_minimum_required(VERSION 3.25.1)

project(AI.Chat.Histories.SQLite VERSION 0.1 LANGUAGES CXX)

find_package(Boost CONFIG REQUIRED COMPONENTS scope)

add_library(ai_chat_histories_sqlite INTERFACE)
add_library(AI::Chat::Histories::SQLite ALIAS ai_chat_histories_sqlite)
target_include_directories(ai_chat_histories_sqlite INTERFACE include)
target_link_libraries(ai_chat_histories_sqlite
  INTERFACE
    SQLite
    Boost::scope
)
target_compile_features(ai_chat_histories_sqlite INTERFACE cxx_std_11)