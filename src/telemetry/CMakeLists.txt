cmake_minimum_required(VERSION 3.25.1)

project(AI.Chat.Telemetry VERSION 0.1)

find_package(EBoost CONFIG REQUIRED)
find_package(opentelemetry-cpp REQUIRED)

add_library(ai_chat_telemetry INTERFACE)
target_compile_features(ai_chat_telemetry INTERFACE cxx_std_17)
target_link_libraries(ai_chat_telemetry
  INTERFACE
    EBoost::eboost
    opentelemetry-cpp::api
    opentelemetry-cpp::otlp_grpc_exporter
    opentelemetry-cpp::otlp_grpc_metrics_exporter
    opentelemetry-cpp::otlp_grpc_log_record_exporter
)
target_include_directories(ai_chat_telemetry INTERFACE include)
add_library(AI::Chat::Telemetry ALIAS ai_chat_telemetry)
