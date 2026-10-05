#ifndef AI_CHAT_MOCKS_GLOBAL_CONFIG_HPP
#define AI_CHAT_MOCKS_GLOBAL_CONFIG_HPP

#include "gmock/gmock.h"

using namespace ::testing;

namespace ai {
namespace chat {
namespace mocks {

class pipe_config {
private:


public:
	pipe_config(
	) = default;
	pipe_config(
		pipe_config const &
	) = delete;
	pipe_config(
		pipe_config &&
	) = default;

	~pipe_config(
	) = default;

	pipe_config
	&operator=(
		pipe_config const &
	) = delete;
	pipe_config
	&operator=(
		pipe_config &&
	) = default;

	MOCK_METHOD(
		bool,
		get_enabled, (
		),
		(const)
	);
};

class thread_config {
private:


public:
	thread_config(
	) = default;
	thread_config(
		thread_config const &
	) = delete;
	thread_config(
		thread_config &&
	) = default;

	~thread_config(
	) = default;

	thread_config
	&operator=(
		thread_config const &
	) = delete;
	thread_config
	&operator=(
		thread_config &&
	) = default;

	MOCK_METHOD(
		pipe_config
		const &, get_pipe, (
		),
		(const)
	);
};

class global_config {
private:


public:
	global_config(
	) = default;
	global_config(
		global_config const &
	) = delete;
	global_config(
		global_config &&
	) = default;

	~global_config(
	) = default;

	global_config
	&operator=(
		global_config const &
	) = delete;
	global_config
	&operator=(
		global_config &&
	) = default;

	MOCK_METHOD(
		thread_config
		const &, get_thread, (
		),
		(const)
	);
};

} // mocks
} // chat
} // ai

#endif
