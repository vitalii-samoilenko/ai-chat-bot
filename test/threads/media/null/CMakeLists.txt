cmake_minimum_required(VERSION 3.25.1)

project(AI.Chat.Test.Threads.Media.Null VERSION 0.1)

find_package(GTest CONFIG REQUIRED)

add_library(_ai_chat_test_threads_media_null INTERFACE)
target_compile_features(_ai_chat_test_threads_media_null INTERFACE cxx_std_20)
target_link_libraries(
	_ai_chat_test_threads_media_null
	INTERFACE
		GTest::gtest
		GTest::gmock
		::AI::Chat::Threads::Media::Null
)
target_sources(_ai_chat_test_threads_media_null
	INTERFACE FILE_SET HEADERS
	BASE_DIRS include
	FILES
	#		include/ai/chat/repositories/pipe_tests.hpp
		include/ai/chat/threads/media/null_tests.hpp
)

add_library(::AI::Chat::Test::Threads::Media::Null ALIAS _ai_chat_test_threads_media_null)
