cmake_minimum_required(VERSION 3.25.1)

project(AI.Threads.Wired VERSION 0.1)

add_library(_ai_threads_wired INTERFACE)
target_compile_features(_ai_threads_wired INTERFACE cxx_std_20)
target_sources(_ai_threads_wired
	INTERFACE FILE_SET HEADERS
	BASE_DIRS include
	FILES
		include/ai/threads/wired.hpp
		include/ai/threads/impl/wired.ipp
)

add_library(::AI::Threads::Wired ALIAS _ai_threads_wired)
