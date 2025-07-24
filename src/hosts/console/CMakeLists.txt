cmake_minimum_required(VERSION 3.25.1)

project(AI.Chat.Hosts.Console VERSION 0.1 LANGUAGES CXX)

find_package(Boost CONFIG REQUIRED COMPONENTS scope json)

add_executable(ai-chat-hosts-console main.cpp)
if(MSVC)
  execute_process(COMMAND "C:/Program Files (x86)/Microsoft Visual Studio/Installer/vswhere.exe"
      -format value -property installationPath
    OUTPUT_VARIABLE VS_INSTALLATION_PATH
    OUTPUT_STRIP_TRAILING_WHITESPACE
    COMMAND_ERROR_IS_FATAL ANY
  )
  execute_process(COMMAND ${VS_INSTALLATION_PATH}/VC/Auxiliary/Build/vcvarsall.bat x64 && rc
      /fo ${CMAKE_CURRENT_BINARY_DIR}/version.res ${PROJECT_SOURCE_DIR}/version.rc
    COMMAND_ERROR_IS_FATAL ANY
  )
  target_link_options(ai-chat-hosts-console PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/version.res)
endif()
target_compile_features(ai-chat-hosts-console PRIVATE cxx_std_20)
target_link_libraries(ai-chat-hosts-console
  PRIVATE
    AI::Chat::Adapters::OpenAI
    AI::Chat::Binders::Console
    AI::Chat::Binders::OpenAI
    AI::Chat::Binders::Twitch
    AI::Chat::Clients::Console
    AI::Chat::Clients::Twitch
    AI::Chat::Clients::Observable
    AI::Chat::Commands
    AI::Chat::Histories::SQLite
    AI::Chat::Histories::Observable
    AI::Chat::Moderators::SQLite
    AI::Chat::Telemetry
    Boost::json
    Boost::scope
)
add_executable(AI::Chat::Hosts::Console ALIAS ai-chat-hosts-console)

install(TARGETS ai-chat-hosts-console)
