cmake_minimum_required(VERSION 3.25.1)

project(AI VERSION 0.1)

if(MSVC)
    add_compile_options(/bigobj)
endif()

add_subdirectory(third_party)

add_subdirectory(src/eboost)
add_subdirectory(src/OpenAI)
add_subdirectory(src/Twitch)
add_subdirectory(src/AI.Chat.Hosts.Console)
