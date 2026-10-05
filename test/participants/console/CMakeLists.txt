cmake_minimum_required(
	VERSION 3.25.1
)

project(
	AI.Chat.Test.Participants.Console
	VERSION 0.1
)

find_package(
	GTest CONFIG REQUIRED
)

add_library(
	_ai_chat_test_participants_console
	INTERFACE
)
target_compile_features(
	_ai_chat_test_participants_console
	INTERFACE cxx_std_20
)
target_link_libraries(
	_ai_chat_test_participants_console
	INTERFACE
		GTest::gtest
		::AI::Chat::Test::Mocks
		::AI::Chat::Model
		::AI::Chat::Participants::Console
)
target_sources(
	_ai_chat_test_participants_console
	INTERFACE FILE_SET HEADERS
	BASE_DIRS include
	FILES
		include/ai/chat/participants/console_tests.hpp
)

add_library(
	::AI::Chat::Test::Participants::Console
	ALIAS _ai_chat_test_participants_console
)
