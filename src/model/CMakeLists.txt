cmake_minimum_required(
	VERSION 3.25.1
)

project(
	AI.Chat.Model
	VERSION 0.1
)

add_library(
	_ai_chat_model
	INTERFACE
)
target_compile_features(
	_ai_chat_model
	INTERFACE cxx_std_20
)
target_sources(
	_ai_chat_model
	INTERFACE FILE_SET HEADERS
	BASE_DIRS include
	FILES
		include/ai/chat/model.hpp
)

add_library(
	::AI::Chat::Model
	ALIAS _ai_chat_model
)
