cmake_minimum_required(VERSION 3.25.1)

project(AI.Chat.Hosts.Console VERSION 0.1 LANGUAGES CXX)

add_executable(AI.Chat.Hosts.Console main.cxx)
target_link_libraries(AI.Chat.Hosts.Console
  PRIVATE
    OpenAI
    Twitch
)
target_compile_features(AI.Chat.Hosts.Console PRIVATE cxx_std_11)

install(TARGETS AI.Chat.Hosts.Console)