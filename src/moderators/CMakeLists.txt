cmake_minimum_required(VERSION 3.25.1)

project(AI.Chat.Moderators VERSION 0.1)

add_library(ai_chat_moderators INTERFACE)
if(EXISTS ${PROJECT_SOURCE_DIR}/sqlite/CMakeLists.txt)
    add_subdirectory(sqlite)
    target_link_libraries(ai_chat_moderators
      INTERFACE
        AI::Chat::Moderators::SQLite
    )
endif()
add_library(AI::Chat::Moderators ALIAS ai_chat_moderators)
