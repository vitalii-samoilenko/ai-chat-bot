cmake_minimum_required(VERSION 3.25.1)

project(AI.Chat.Histories VERSION 0.1)

add_library(ai_chat_histories INTERFACE)
add_library(AI::Chat::Histories ALIAS ai_chat_histories)

add_subdirectory(observable)
target_link_libraries(ai_chat_histories
    INTERFACE
    AI::Chat::Histories::Observable
)

if(EXISTS ${PROJECT_SOURCE_DIR}/sqlite/CMakeLists.txt)
    add_subdirectory(sqlite)
    target_link_libraries(ai_chat_histories
      INTERFACE
        AI::Chat::Histories::SQLite
    )
endif()