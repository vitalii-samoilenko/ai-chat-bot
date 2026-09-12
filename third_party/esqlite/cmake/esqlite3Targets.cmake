include(${CMAKE_CURRENT_LIST_DIR}/_esqlite3Targets.cmake)

add_library(::ESQLite3 ALIAS ::ESQLite3::_esqlite3)
