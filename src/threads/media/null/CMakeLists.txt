cmake_minimum_required(VERSION 3.25.1)

project(AI.Chat.Threads.Media.Null VERSION 0.1)

add_library(_ai_chat_threads_media_null INTERFACE)
target_compile_features(_ai_chat_threads_media_null INTERFACE cxx_std_20)
target_sources(_ai_chat_threads_media_null
	INTERFACE FILE_SET HEADERS
	BASE_DIRS include
	FILES
		include/ai/chat/threads/media/null.hpp
		include/ai/chat/threads/media/impl/null.ipp
)

add_library(::AI::Chat::Threads::Media::Null ALIAS _ai_chat_threads_media_null)
