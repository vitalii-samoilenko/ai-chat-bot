cmake_minimum_required(VERSION 3.25.1)

project(AI.Chat.Binders VERSION 0.1)

add_library(ai_chat_binders INTERFACE)
if(EXISTS ${PROJECT_SOURCE_DIR}/openai/CMakeLists.txt)
    add_subdirectory(openai)
    target_link_libraries(ai_chat_binders
        INTERFACE
            AI::Chat::Binders::OpenAI
    )
endif()
if(EXISTS ${PROJECT_SOURCE_DIR}/twitch/CMakeLists.txt)
    add_subdirectory(twitch)
    target_link_libraries(ai_chat_binders
        INTERFACE
            AI::Chat::Binders::Twitch
    )
endif()
add_library(AI::Chat::Binders ALIAS ai_chat_binders)