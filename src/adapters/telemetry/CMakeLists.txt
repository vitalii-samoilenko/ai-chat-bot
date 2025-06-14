cmake_minimum_required(VERSION 3.25.1)

project(AI.Chat.Adapters.Telemetry VERSION 0.1 LANGUAGES CXX)

add_library(ai_chat_adapters_telemetry INTERFACE)
add_library(AI::Chat::Adapters::Telemetry ALIAS ai_chat_adapters_telemetry)
target_include_directories(ai_chat_adapters_telemetry INTERFACE include)
target_link_libraries(ai_chat_adapters_telemetry
  INTERFACE
    opentelemetry-cpp::api
)
target_compile_features(ai_chat_adapters_telemetry INTERFACE cxx_std_17)