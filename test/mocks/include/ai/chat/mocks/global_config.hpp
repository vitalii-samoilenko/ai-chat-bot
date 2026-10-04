#ifndef AI_CHAT_MOCKS_GLOBAL_CONFIG_HPP
#define AI_CHAT_MOCKS_GLOBAL_CONFIG_HPP

#include "gmock/gmock.h"

using namespace ::testing;

namespace ai {
namespace chat {
namespace mocks {

class PipeConfig {
private:


public:
	PipeConfig(
	) = default;
	PipeConfig(
		PipeConfig const &
	) = delete;
	PipeConfig(
		PipeConfig &&
	) = default;

	~PipeConfig(
	) = default;

	PipeConfig
	&operator=(
		PipeConfig const &
	) = delete;
	PipeConfig
	&operator=(
		PipeConfig &&
	) = default;

	MOCK_METHOD(
		bool,
		get_enabled, (
		),
		(const)
	);
};

class ThreadConfig {
private:


public:
	ThreadConfig(
	) = default;
	ThreadConfig(
		ThreadConfig const &
	) = delete;
	ThreadConfig(
		ThreadConfig &&
	) = default;

	~ThreadConfig(
	) = default;

	ThreadConfig
	&operator=(
		ThreadConfig const &
	) = delete;
	ThreadConfig
	&operator=(
		ThreadConfig &&
	) = default;

	MOCK_METHOD(
		PipeConfig
		const &, get_pipe, (
		),
		(const)
	);
};

class GlobalConfig {
private:


public:
	GlobalConfig(
	) = default;
	GlobalConfig(
		GlobalConfig const &
	) = delete;
	GlobalConfig(
		GlobalConfig &&
	) = default;

	~GlobalConfig(
	) = default;

	GlobalConfig
	&operator=(
		GlobalConfig const &
	) = delete;
	GlobalConfig
	&operator=(
		GlobalConfig &&
	) = default;

	MOCK_METHOD(
		ThreadConfig
		const &, get_thread, (
		),
		(const)
	);
};

} // mocks
} // chat
} // ai

#endif
