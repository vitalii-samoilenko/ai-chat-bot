cmake_minimum_required(VERSION 3.25.1)

project(AI.Chat.Hosts.Console VERSION 0.1 LANGUAGES CXX)

add_executable(ai-chat-hosts-console main.cpp)
if(MSVC)
	if(MSVC_TOOLSET_VERSION EQUAL 143)
		set(BUILD_TOOLS_INSTALLATION_PATH "C:/Program Files (x86)/Microsoft Visual Studio/2022/BuildTools")
	endif()
	if(NOT EXISTS ${BUILD_TOOLS_INSTALLATION_PATH})
		message(FATAL_ERROR "Toolset is not supported")
	endif()
	execute_process(COMMAND ${BUILD_TOOLS_INSTALLATION_PATH}/VC/Auxiliary/Build/vcvarsall.bat x64 && rc
			/fo ${CMAKE_CURRENT_BINARY_DIR}/version.res ${PROJECT_SOURCE_DIR}/version.rc
		COMMAND_ERROR_IS_FATAL ANY
	)
	target_link_options(ai-chat-hosts-console PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/version.res)
endif()
target_compile_features(ai-chat-hosts-console PRIVATE cxx_std_20)
target_link_libraries(ai-chat-hosts-console
	PRIVATE
		AI::Chat::Telemetry
)
add_executable(AI::Chat::Hosts::Console ALIAS ai-chat-hosts-console)

install(TARGETS ai-chat-hosts-console)
