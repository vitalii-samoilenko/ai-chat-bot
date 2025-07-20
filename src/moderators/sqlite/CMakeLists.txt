cmake_minimum_required(VERSION 3.25.1)

project(AI.Chat.Moderators.SQLite VERSION 0.1 LANGUAGES CXX)

find_package(ESQLite3 REQUIRED)
find_package(SQLite3 REQUIRED)

add_library(ai_chat_moderators_sqlite INTERFACE)
target_compile_features(ai_chat_moderators_sqlite INTERFACE cxx_std_20)
target_link_libraries(ai_chat_moderators_sqlite
  INTERFACE
    ESQLite::esqlite3
    SQLite::SQLite3
)
target_include_directories(ai_chat_moderators_sqlite INTERFACE include)
add_library(AI::Chat::Moderators::SQLite ALIAS ai_chat_moderators_sqlite)
