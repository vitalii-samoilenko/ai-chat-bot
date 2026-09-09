cmake_minimum_required(VERSION 3.25.1)

project(ESTD VERSION 0.1 LANGUAGES CXX)

add_library(estd INTERFACE)
target_compile_features(estd INTERFACE cxx_std_11)
target_include_directories(estd
	INTERFACE
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
		$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)
add_library(ESTD ALIAS estd)

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)
install(TARGETS estd
	EXPORT estdTargets
	PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR} COMPONENT Development
)
configure_package_config_file(
	${CMAKE_CURRENT_SOURCE_DIR}/ESTDConfig.cmake.in
	${CMAKE_CURRENT_BINARY_DIR}/ESTDConfig.cmake
	INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/ESTD
)
write_basic_package_version_file(
	${CMAKE_CURRENT_BINARY_DIR}/ESTDConfigVersion.cmake
	VERSION 0.1.0
	COMPATIBILITY SameMajorVersion
)
install(
	EXPORT estdTargets
	DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/ESTD
	NAMESPACE ::
)
install(
	FILES
		${CMAKE_CURRENT_BINARY_DIR}/ESTDConfig.cmake
		${CMAKE_CURRENT_BINARY_DIR}/ESTDConfigVersion.cmake
	DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/ESTD
)
install(
	DIRECTORY
		${CMAKE_CURRENT_SOURCE_DIR}/include/
	DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)
