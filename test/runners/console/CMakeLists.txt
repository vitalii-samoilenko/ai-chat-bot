cmake_minimum_required(
	VERSION 3.25.1
)

project(
	AI.Chat.Test.Runners.Console
	VERSION 0.1
	LANGUAGES CXX
)

find_package(
	GTest CONFIG REQUIRED
)

add_executable(
	ai-chat-test-runners-console
		main.cpp
)
target_compile_features(
	ai-chat-test-runners-console
	PRIVATE cxx_std_20
)
target_link_libraries(
	ai-chat-test-runners-console
	PRIVATE
		GTest::gtest
		::AI::Chat::Test::Threads::Backed
		::AI::Chat::Test::Threads::Media::Null
		::AI::Chat::Test::Participants::Console
)

add_custom_command(
	TARGET ai-chat-test-runners-console
	POST_BUILD COMMAND ai-chat-test-runners-console
)
