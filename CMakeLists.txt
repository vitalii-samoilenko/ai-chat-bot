cmake_minimum_required(VERSION 3.28.2)

project(AI VERSION 0.1)

add_library(compiler_flags INTERFACE)
target_compile_features(compiler_flags INTERFACE cxx_std_11)

add_subdirectory(boost)

add_subdirectory(src/AI.Chat.Hosts.Console)

target_link_libraries(AI.Chat.Hosts.Console PUBLIC compiler_flags)