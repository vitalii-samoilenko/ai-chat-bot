cmake_minimum_required(VERSION 3.25.1)

project(AI.Chat.Participants.Console VERSION 0.1)

find_package(re2 CONFIG REQUIRED)

add_library(_ai_chat_participants_console INTERFACE)
target_compile_features(_ai_chat_participants_console INTERFACE cxx_std_20)
target_link_libraries(_ai_chat_participants_console INTERFACE re2::re2)
target_sources(_ai_chat_participants_console
	INTERFACE FILE_SET HEADERS
	BASE_DIRS include
	FILES
		include/ai/participants/
)

add_library(::AI::Chat::Participants::Console ALIAS _ai_chat_participants_console)
