#ifndef AI_CHAT_REPOSITORIES_PIPE_TESTS_HPP
#define AI_CHAT_REPOSITORIES_PIPE_TESTS_HPP

#include <utility>

#include "gtest/gtest.h"

#include "ai/chat/mocks/communication_cluster.hpp"
#include "ai/chat/mocks/global_config.hpp"

#include "ai/chat/model.hpp"
#include "ai/chat/repositories/pipe.hpp"

using namespace ::testing;

namespace ai {
namespace chat {
namespace repositories {

TEST(PipeRepositoryTests, EmptyRange) {
	string_t partition{};

	mocks::GlobalConfig globalConfig{};
	mocks::CommunicationCluster communicationCluster{};

	pipe<
		mocks::GlobalConfig,
		mocks::CommunicationCluster
	> target{
		partition,
		globalConfig,
		communicationCluster
	};

	auto begin = target.begin();
	auto end = target.end();

	ASSERT_THAT(
		begin,
		Eq(::std::ref(end))
	);
};
TEST(PipeRepositoryTests, SpawnsChannel) {
	string_t partition{ "test" };
	string_t slot{ "pipe" };

	mocks::PipeConfig pipeConfig{};
	mocks::ThreadConfig threadConfig{};
	mocks::GlobalConfig globalConfig{};
	mocks::CommunicationCluster communicationCluster{};

	EXPECT_CALL(
		pipeConfig, get_enabled(
		)
	).WillRepeatedly(
		Return(true)
	);
	EXPECT_CALL(
		threadConfig, get_pipe(
		)
	).WillRepeatedly(
		[&]()->mocks::PipeConfig const & {
			return pipeConfig;
		}
	);
	EXPECT_CALL(
		globalConfig, get_thread(
		)
	).WillRepeatedly(
		[&]()->mocks::ThreadConfig const & {
			return threadConfig;
		}
	);

	pipe<
		mocks::GlobalConfig,
		mocks::CommunicationCluster
	> target{
		partition,
		globalConfig,
		communicationCluster
	};

	auto channel = target.find(
		partition,
		slot
	);
	auto end = target.end();

	ASSERT_THAT(
		channel,
		Ne(::std::ref(end))
	);
};
TEST(PipeRepositoryTests, CachesChannel) {
	string_t partition{ "test" };
	string_t slot_a{ "pipe_a" };
	string_t slot_b{ "pipe_b" };

	mocks::PipeConfig pipeConfig{};
	mocks::ThreadConfig threadConfig{};
	mocks::GlobalConfig globalConfig{};
	mocks::CommunicationCluster communicationCluster{};

	EXPECT_CALL(
		pipeConfig, get_enabled(
		)
	).WillRepeatedly(
		Return(true)
	);
	EXPECT_CALL(
		threadConfig, get_pipe(
		)
	).WillRepeatedly(
		[&]()->mocks::PipeConfig const & {
			return pipeConfig;
		}
	);
	EXPECT_CALL(
		globalConfig, get_thread(
		)
	).WillRepeatedly(
		[&]()->mocks::ThreadConfig const & {
			return threadConfig;
		}
	);

	pipe<
		mocks::GlobalConfig,
		mocks::CommunicationCluster
	> target{
		partition,
		globalConfig,
		communicationCluster
	};

	{
		auto channel_a = target.find(
			partition,
			slot_a
		);
		auto cached_a = target.find(
			partition,
			slot_a
		);

		ASSERT_THAT(
			channel_a,
			Eq(::std::ref(cached_a))
		);
	}
	{
		auto channel_b = target.find(
			partition,
			slot_b
		);
		auto cached_b = target.find(
			partition,
			slot_b
		);

		ASSERT_THAT(
			channel_b,
			Eq(::std::ref(cached_b))
		);
	}

	auto current = target.begin();
	auto end = target.end();

	ASSERT_THAT(
		current,
		Ne(::std::ref(end))
	);
	ASSERT_THAT(
		++current,
		Ne(::std::ref(end))
	);
	ASSERT_THAT(
		++current,
		Eq(::std::ref(end))
	);
};
TEST(PipeRepositoryTests, ControlledByConfig) {
	string_t partition{ "test" };
	string_t slot{ "pipe" };

	mocks::PipeConfig pipeConfig{};
	mocks::ThreadConfig threadConfig{};
	mocks::GlobalConfig globalConfig{};
	mocks::CommunicationCluster communicationCluster{};

	EXPECT_CALL(
		pipeConfig, get_enabled(
		)
	).WillRepeatedly(
		Return(false)
	);
	EXPECT_CALL(
		threadConfig, get_pipe(
		)
	).WillRepeatedly(
		[&]()->mocks::PipeConfig const & {
			return pipeConfig;
		}
	);
	EXPECT_CALL(
		globalConfig, get_thread(
		)
	).WillRepeatedly(
		[&]()->mocks::ThreadConfig const & {
			return threadConfig;
		}
	);

	pipe<
		mocks::GlobalConfig,
		mocks::CommunicationCluster
	> target{
		partition,
		globalConfig,
		communicationCluster
	};

	auto channel = target.find(
		partition,
		slot
	);
	auto end = target.end();

	ASSERT_THAT(
		channel,
		Eq(::std::ref(end))
	);
};
TEST(PipeRepositoryTests, ValidatesPartition) {
	string_t partition_a{ "test_a" };
	string_t partition_b{ "test_b" };
	string_t slot{ "pipe" };

	mocks::PipeConfig pipeConfig{};
	mocks::ThreadConfig threadConfig{};
	mocks::GlobalConfig globalConfig{};
	mocks::CommunicationCluster communicationCluster{};

	EXPECT_CALL(
		pipeConfig, get_enabled(
		)
	).WillRepeatedly(
		Return(true)
	);
	EXPECT_CALL(
		threadConfig, get_pipe(
		)
	).WillRepeatedly(
		[&]()->mocks::PipeConfig const & {
			return pipeConfig;
		}
	);
	EXPECT_CALL(
		globalConfig, get_thread(
		)
	).WillRepeatedly(
		[&]()->mocks::ThreadConfig const & {
			return threadConfig;
		}
	);

	pipe<
		mocks::GlobalConfig,
		mocks::CommunicationCluster
	> target{
		partition_a,
		globalConfig,
		communicationCluster
	};

	auto channel = target.find(
		partition_b,
		slot
	);
	auto end = target.end();

	ASSERT_THAT(
		channel,
		Eq(::std::ref(end))
	);
};

} // repositories
} // chat
} // ai

#endif
