cmake_minimum_required(VERSION 3.25.1)

project(ESQLite VERSION 0.1 LANGUAGES CXX)

find_package(re2 REQUIRED)
find_package(SQLite3 REQUIRED)

add_library(_esqlite3 INTERFACE)
target_compile_features(_esqlite3 INTERFACE cxx_std_17)
target_link_libraries(_esqlite3
	INTERFACE
		re2::re2
		SQLite::SQLite3
)
target_sources(_esqlite3
	INTERFACE FILE_SET HEADERS
	BASE_DIRS include
	FILES
		include/esqlite3/ensure_success.hpp
		include/esqlite3/regexp.hpp
		include/esqlite3.hpp
)

add_library(ESQLite3 ALIAS _esqlite3)

include(GNUInstallDirs)
install(TARGETS _esqlite3
	EXPORT _esqlite3Targets
	FILE_SET HEADERS DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)
install(
	EXPORT _esqlite3Targets
	DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/ESQLite3
	NAMESPACE ::ESQLite3::
)

include(CMakePackageConfigHelpers)
configure_package_config_file(
	${CMAKE_CURRENT_SOURCE_DIR}/cmake/ESQLite3Config.cmake.in
	${CMAKE_CURRENT_BINARY_DIR}/cmake/ESQLite3Config.cmake
	INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/ESQLite3
)
write_basic_package_version_file(
	${CMAKE_CURRENT_BINARY_DIR}/cmake/ESQLite3ConfigVersion.cmake
	VERSION 0.1.0
	COMPATIBILITY SameMajorVersion
)
install(
	FILES
		${CMAKE_CURRENT_BINARY_DIR}/cmake/ESQLite3Config.cmake
		${CMAKE_CURRENT_BINARY_DIR}/cmake/ESQLite3ConfigVersion.cmake
	DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/ESQLite3
)
