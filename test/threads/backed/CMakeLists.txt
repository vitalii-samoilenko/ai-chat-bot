cmake_minimum_required(VERSION 3.25.1)

project(AI.Chat.Test.Threads.Backed VERSION 0.1)

find_package(GTest CONFIG REQUIRED)

add_library(_ai_chat_test_threads_backed INTERFACE)
target_compile_features(_ai_chat_test_threads_backed INTERFACE cxx_std_20)
target_link_libraries(_ai_chat_test_threads_backed
	INTERFACE
		GTest::gtest
		GTest::gmock
		::AI::Chat::Threads::Backed
)
target_sources(_ai_chat_test_threads_backed
	INTERFACE FILE_SET HEADERS
	BASE_DIRS include
	FILES
		include/ai/threads/backed_tests.hpp
)

add_library(::AI::Chat::Test::Threads::Backed ALIAS _ai_chat_test_threads_backed)
