cmake_minimum_required(VERSION 3.25.1)

project(AI.Chat.Test VERSION 0.1 LANGUAGES CXX)

find_package(GTest CONFIG REQUIRED)

add_subdirectory(threads)

add_executable(ai-chat-test main.cpp)
target_compile_features(ai-chat-test PRIVATE cxx_std_20)
target_link_libraries(ai-chat-test
	PRIVATE
		GTest::gtest
		::AI::Chat::Test::Threads::Media::Null
)
