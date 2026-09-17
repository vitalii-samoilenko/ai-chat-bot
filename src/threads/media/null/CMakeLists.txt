cmake_minimum_required(VERSION 3.25.1)

project(AI.Threads.Media.Null VERSION 0.1)

add_library(_ai_threads_media_null INTERFACE)
target_compile_features(_ai_threads_media_null INTERFACE cxx_std_20)
target_sources(_ai_threads_media_null
	INTERFACE FILE_SET HEADERS
	BASE_DIRS include
	FILES
		include/ai/threads/media/null.hpp
		include/ai/threads/media/impl/null.ipp
)

add_library(::AI::Threads::Media::Null ALIAS _ai_threads_media_null)
