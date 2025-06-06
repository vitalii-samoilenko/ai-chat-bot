cmake_minimum_required(VERSION 3.28.2)

project(AI VERSION 0.1)

find_package(OpenSSL)

add_subdirectory(libs/boost)

add_subdirectory(src/eboost)
add_subdirectory(src/OpenAI)
add_subdirectory(src/AI.Chat.Hosts.Console)
