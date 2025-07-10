cmake_minimum_required(VERSION 3.25.1)

project(AI VERSION 0.1)

if(MSVC)
    add_compile_options(/bigobj)
    set(CMAKE_MSVC_RUNTIME_LIBRARY MultiThreaded$<$<CONFIG:Debug>:Debug>)
    #add_compile_options(/GR-)
endif()

add_subdirectory(third_party)

add_subdirectory(src/clients)
add_subdirectory(src/adapters)
add_subdirectory(src/histories)
add_subdirectory(src/moderators)
add_subdirectory(src/commands)
add_subdirectory(src/binders)
add_subdirectory(src/hosts/console)
