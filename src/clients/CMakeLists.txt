cmake_minimum_required(VERSION 3.25.1)

project(AI.Chat.Clients VERSION 0.1)

add_library(ai_chat_clients INTERFACE)
add_library(AI::Chat::Clients ALIAS ai_chat_clients)

add_subdirectory(observable)
target_link_libraries(ai_chat_clients
  INTERFACE
    AI::Chat::Clients::Observable
)
if(EXISTS ${PROJECT_SOURCE_DIR}/twitch/CMakeLists.txt)
    add_subdirectory(twitch)
    target_link_libraries(ai_chat_clients
      INTERFACE
        AI::Chat::Clients::Twitch
    )
endif()
