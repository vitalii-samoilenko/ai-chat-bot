cmake_minimum_required(VERSION 3.25.1)

project(AI VERSION 0.1)

add_subdirectory(libs/boost)

add_subdirectory(src/eboost)
add_subdirectory(src/OpenAI)
add_subdirectory(src/Twitch)
add_subdirectory(src/AI.Chat.Hosts.Console)
