cmake_minimum_required(
	VERSION 3.25.1
)

project(
	AI.Chat.Test.Mocks
	VERSION 0.1
	LANGUAGES CXX
)

find_package(
	GTest CONFIG REQUIRED
)

add_library(
	_ai_chat_test_mocks
	INTERFACE
)
target_compile_features(
	_ai_chat_test_mocks
	INTERFACE cxx_std_20
)
target_link_libraries(
	_ai_chat_test_mocks
	INTERFACE
		GTest::gmock
		::AI::Chat::Model
)
target_sources(
	_ai_chat_test_mocks
	INTERFACE FILE_SET HEADERS
	BASE_DIRS include
	FILES
		include/ai/chat/mocks/communication_cluster.hpp
)

add_library(
	::AI::Chat::Test::Mocks
	ALIAS _ai_chat_test_mocks
)
