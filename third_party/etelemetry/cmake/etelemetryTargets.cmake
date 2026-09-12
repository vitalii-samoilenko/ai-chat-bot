include(${CMAKE_CURRENT_LIST_DIR}/_etelemetryTargets.cmake)

add_library(::ETelemetry ALIAS ::ETelemetry::_etelemetry)
