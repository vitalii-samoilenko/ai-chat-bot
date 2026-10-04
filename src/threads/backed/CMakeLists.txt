cmake_minimum_required(
	VERSION 3.25.1
)

project(
	AI.Chat.Threads.Backed
	VERSION 0.1
)

find_package(
	ESTD CONFIG REQUIRED
)

add_library(
	_ai_chat_threads_backed
	INTERFACE
)
target_compile_features(
	_ai_chat_threads_backed
	INTERFACE cxx_std_23
)
target_link_libraries(
	_ai_chat_threads_backed
	INTERFACE
		::ESTD
		::AI::Chat::Model
)
target_sources(
	_ai_chat_threads_backed
	INTERFACE FILE_SET HEADERS
	BASE_DIRS include
	FILES
		include/ai/threads/backed.hpp
		include/ai/threads/impl/backed.ipp
)

add_library(
	::AI::Chat::Threads::Backed
	ALIAS _ai_chat_threads_backed
)
