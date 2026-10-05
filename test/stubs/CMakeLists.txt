cmake_minimum_required(
	VERSION 3.25.1
)

project(
	AI.Chat.Test.Stubs
	VERSION 0.1
	LANGUAGES CXX
)

add_library(
	_ai_chat_test_stubs
	INTERFACE
)
target_compile_features(
	_ai_chat_test_stubs
	INTERFACE cxx_std_20
)
target_sources(
	_ai_chat_test_stubs
	INTERFACE FILE_SET HEADERS
	BASE_DIRS include
	FILES
		include/ai/chat/stubs/console.hpp
		include/ai/chat/stubs/impl/console.ipp
		include/ai/chat/stubs/impl/linux_console.ipp
)

add_library(
	::AI::Chat::Test::Stubs
	ALIAS _ai_chat_test_stubs
)
