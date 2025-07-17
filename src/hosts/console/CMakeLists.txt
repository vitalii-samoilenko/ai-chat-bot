cmake_minimum_required(VERSION 3.25.1)

project(AI.Chat.Hosts.Console VERSION 0.1 LANGUAGES CXX)

find_package(Boost CONFIG REQUIRED COMPONENTS scope json)
find_package(EBoost CONFIG REQUIRED)
find_package(opentelemetry-cpp REQUIRED)

add_executable(ai-chat-hosts-console main.cpp)
target_compile_features(ai-chat-hosts-console PRIVATE cxx_std_20)
target_link_libraries(ai-chat-hosts-console
  PRIVATE
    AI::Chat::Adapters::OpenAI
    AI::Chat::Binders::Twitch
    AI::Chat::Binders::OpenAI
    AI::Chat::Clients::Twitch
    AI::Chat::Clients::Observable
    AI::Chat::Commands
    AI::Chat::Histories::SQLite
    AI::Chat::Histories::Observable
    AI::Chat::Moderators::SQLite
    Boost::json
    Boost::scope
    EBoost::eboost
    opentelemetry-cpp::otlp_grpc_exporter
    opentelemetry-cpp::otlp_grpc_metrics_exporter
    opentelemetry-cpp::otlp_grpc_log_record_exporter
)
add_executable(AI::Chat::Hosts::Console ALIAS ai-chat-hosts-console)

install(TARGETS ai-chat-hosts-console)
