cmake_minimum_required(VERSION 3.25.1)

project(EBoost VERSION 0.1 LANGUAGES CXX)

find_package(Boost CONFIG REQUIRED COMPONENTS asio beast json)

add_library(eboost INTERFACE)
target_compile_features(eboost INTERFACE cxx_std_11)
target_link_libraries(eboost
  INTERFACE
    Boost::asio
    Boost::beast
    Boost::json
)
target_include_directories(eboost
  INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)
add_library(EBoost ALIAS eboost)

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)
install(TARGETS eboost
  EXPORT eboostTargets
  PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR} COMPONENT Development
)
configure_package_config_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/EBoostConfig.cmake.in
  ${CMAKE_CURRENT_BINARY_DIR}/EBoostConfig.cmake
  INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/EBoost
)
write_basic_package_version_file(
  ${CMAKE_CURRENT_BINARY_DIR}/EBoostConfigVersion.cmake
  VERSION 0.1.0
  COMPATIBILITY SameMajorVersion
)
install(
  EXPORT eboostTargets
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/EBoost
  NAMESPACE EBoost::
)
install(
  FILES
    ${CMAKE_CURRENT_BINARY_DIR}/EBoostConfig.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/EBoostConfigVersion.cmake
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/EBoost
)
install(
  DIRECTORY
    ${CMAKE_CURRENT_SOURCE_DIR}/include/
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)
