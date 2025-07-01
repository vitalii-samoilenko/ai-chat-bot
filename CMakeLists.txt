cmake_minimum_required(VERSION 3.25.1)

project(AI VERSION 0.1)

if(MSVC)
    add_compile_options(/bigobj)
endif()

add_subdirectory(third_party)

add_subdirectory(src/clients/twitch)
add_subdirectory(src/clients/observable)
add_subdirectory(src/adapters/openai)
add_subdirectory(src/histories/sqlite)
add_subdirectory(src/histories/observable)
add_subdirectory(src/moderators/sqlite)
add_subdirectory(src/binders/openai)
add_subdirectory(src/binders/twitch)
add_subdirectory(src/hosts/console)
