cmake_minimum_required(VERSION 3.25.1)

project(AI.Chat.Adapters VERSION 0.1)

add_library(ai_chat_adapters INTERFACE)
add_library(AI::Chat::Adapters ALIAS ai_chat_adapters)

if(EXISTS ${PROJECT_SOURCE_DIR}/openai/CMakeLists.txt)
    add_subdirectory(openai)
    target_link_libraries(ai_chat_adapters
      INTERFACE
        AI::Chat::Adapters::OpenAI
    )
endif()