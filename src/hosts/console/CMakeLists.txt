cmake_minimum_required(VERSION 3.25.1)

project(AI.Chat.Hosts.Console VERSION 0.1 LANGUAGES CXX)

add_executable(ai-chat-hosts-console main.cpp)
add_executable(AI::Chat::Hosts::Console ALIAS ai-chat-hosts-console)
target_link_libraries(ai-chat-hosts-console
  PRIVATE
    Boost::json
    AI::Chat::Clients::Twitch
    AI::Chat::Clients::Observable
    AI::Chat::Adapters::OpenAI
    AI::Chat::Histories::SQLite
    AI::Chat::Histories::Observable
    AI::Chat::Moderators::SQLite
    AI::Chat::Binders::Twitch
    AI::Chat::Binders::OpenAI
    opentelemetry-cpp::ostream_metrics_exporter
    opentelemetry-cpp::ostream_span_exporter
    opentelemetry-cpp::ostream_log_record_exporter
)
target_compile_features(ai-chat-hosts-console PRIVATE cxx_std_17)

install(TARGETS ai-chat-hosts-console)
