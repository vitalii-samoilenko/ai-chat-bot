cmake_minimum_required(VERSION 3.25.1)

project(ETelemetry VERSION 0.1)

find_package(ESTD CONFIG REQUIRED)
find_package(opentelemetry-cpp REQUIRED)

add_library(_etelemetry INTERFACE)
target_compile_features(_etelemetry INTERFACE cxx_std_17)
target_link_libraries(_etelemetry
	INTERFACE
		::ESTD
		opentelemetry-cpp::api
		opentelemetry-cpp::otlp_grpc_exporter
		opentelemetry-cpp::otlp_grpc_metrics_exporter
		opentelemetry-cpp::otlp_grpc_log_record_exporter
)
target_sources(_etelemetry
	INTERFACE FILE_SET HEADERS
	BASE_DIRS include
	FILES
		include/etelemetry/common.hpp
		include/etelemetry/init.hpp
		include/etelemetry/logger.hpp
		include/etelemetry/meter.hpp
		include/etelemetry/system.hpp
		include/etelemetry/tracer.hpp
		include/etelemetry.hpp
)

add_library(ETelemetry ALIAS _etelemetry)

include(GNUInstallDirs)
install(
	TARGETS _etelemetry
	EXPORT _etelemetryTargets
	FILE_SET HEADERS DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)
install(
	EXPORT _etelemetryTargets
	DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/ETelemetry
	NAMESPACE ::ETelemetry::
)

include(CMakePackageConfigHelpers)
configure_package_config_file(
	${CMAKE_CURRENT_SOURCE_DIR}/cmake/ETelemetryConfig.cmake.in
	${CMAKE_CURRENT_BINARY_DIR}/cmake/ETelemetryConfig.cmake
	INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/ETelemetry
)
write_basic_package_version_file(
	${CMAKE_CURRENT_BINARY_DIR}/cmake/ETelemetryConfigVersion.cmake
	VERSION 0.1.0
	COMPATIBILITY SameMajorVersion
)
install(
	FILES
		${CMAKE_CURRENT_BINARY_DIR}/cmake/ETelemetryConfig.cmake
		${CMAKE_CURRENT_BINARY_DIR}/cmake/ETelemetryConfigVersion.cmake
		${CMAKE_CURRENT_SOURCE_DIR}/cmake/etelemetryTargets.cmake
	DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/ETelemetry
)
