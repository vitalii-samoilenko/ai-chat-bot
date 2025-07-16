cmake_minimum_required(VERSION 3.25.1)

project(ESQLite VERSION 0.1 LANGUAGES CXX)

find_package(re2 REQUIRED)
find_package(SQLite3 REQUIRED)

add_library(esqlite3 INTERFACE)
target_compile_features(esqlite3 INTERFACE cxx_std_11)
target_link_libraries(esqlite3
  INTERFACE
    re2::re2
    SQLite::SQLite3
)
target_include_directories(esqlite3
  INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)
add_library(ESQLite3 ALIAS esqlite3)

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)
install(TARGETS esqlite3
  EXPORT esqlite3Targets
  PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR} COMPONENT Development
)
configure_package_config_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/ESQLite3Config.cmake.in
  ${CMAKE_CURRENT_BINARY_DIR}/ESQLite3Config.cmake
  INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/ESQLite3
)
write_basic_package_version_file(
  ${CMAKE_CURRENT_BINARY_DIR}/ESQLite3ConfigVersion.cmake
  VERSION 0.1.0
  COMPATIBILITY SameMajorVersion
)
install(
  EXPORT esqlite3Targets
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/ESQLite3
  NAMESPACE ESQLite::
)
install(
  FILES
    ${CMAKE_CURRENT_BINARY_DIR}/ESQLite3Config.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/ESQLite3ConfigVersion.cmake
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/ESQLite3
)
install(
  DIRECTORY
    ${CMAKE_CURRENT_SOURCE_DIR}/include/
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)
