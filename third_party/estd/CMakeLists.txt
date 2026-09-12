cmake_minimum_required(VERSION 3.25.1)

project(ESTD VERSION 0.1 LANGUAGES CXX)

add_library(_estd INTERFACE)
target_compile_features(_estd INTERFACE cxx_std_11)
target_sources(_estd
	INTERFACE FILE_SET HEADERS
	BASE_DIRS include
	FILES
		include/estd/system/cpu.hpp
		include/estd/system/impl/linux.ipp
		include/estd/system/impl/windows.ipp
		include/estd/system/memory.hpp
		include/estd/system.hpp
)

add_library(ESTD ALIAS _estd)

include(GNUInstallDirs)
install(
	TARGETS _estd
	EXPORT _estdTargets
	FILE_SET HEADERS DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)
install(
	EXPORT _estdTargets
	DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/ESTD
	NAMESPACE ::ESTD::
)

include(CMakePackageConfigHelpers)
configure_package_config_file(
	${CMAKE_CURRENT_SOURCE_DIR}/cmake/ESTDConfig.cmake.in
	${CMAKE_CURRENT_BINARY_DIR}/cmake/ESTDConfig.cmake
	INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/ESTD
)
write_basic_package_version_file(
	${CMAKE_CURRENT_BINARY_DIR}/cmake/ESTDConfigVersion.cmake
	VERSION 0.1.0
	COMPATIBILITY SameMajorVersion
)
install(
	FILES
		${CMAKE_CURRENT_BINARY_DIR}/cmake/ESTDConfig.cmake
		${CMAKE_CURRENT_BINARY_DIR}/cmake/ESTDConfigVersion.cmake
		${CMAKE_CURRENT_SOURCE_DIR}/cmake/estdTargets.cmake
	DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/ESTD
)
