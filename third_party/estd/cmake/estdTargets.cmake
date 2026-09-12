include(${CMAKE_CURRENT_LIST_DIR}/_estdTargets.cmake)

add_library(::ESTD ALIAS ::ESTD::_estd)
