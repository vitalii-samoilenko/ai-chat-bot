cmake_minimum_required(VERSION 3.25.1)

project(AI VERSION 0.1)

if(MSVC)
    add_compile_options(/bigobj)
endif()

add_subdirectory(third_party)

add_subdirectory(src/adapters/common)
add_subdirectory(src/adapters/openai)
add_subdirectory(src/adapters/telemetry)
add_subdirectory(src/hosts/console)
