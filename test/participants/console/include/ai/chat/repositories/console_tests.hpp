#ifndef AI_CHAT_REPOSITORIES_CONSOLE_TESTS_HPP
#define AI_CHAT_REPOSITORIES_CONSOLE_TESTS_HPP

#include <utility>

#include "gtest/gtest.h"

#include "ai/chat/mocks/global_config.hpp"
#include "ai/chat/mocks/communication_cluster.hpp"
#include "ai/chat/mocks/runtime.hpp"

#include "ai/chat/model.hpp"
#include "ai/chat/repositories/console.hpp"

using namespace ::testing;

namespace ai {
namespace chat {
namespace repositories {

TEST(ConsoleRepositoryTests, EmptyRange) {
	string_t partition{};

	mocks::global_config globalConfig{};
	mocks::runtime runtime{};
	mocks::communication_cluster communicationCluster{};

	repositories::console<
		mocks::global_config,
		mocks::runtime,
		mocks::communication_cluster
	> target{
		partition,
		globalConfig,
		runtime,
		communicationCluster
	};

	auto begin = target.begin();
	auto end = target.end();

	ASSERT_THAT(
		begin,
		Eq(::std::ref(end))
	);
};
TEST(ConsoleRepositoryTests, SpawnsReceiver) {
	string_t partition{ "test" };
	string_t slot{ "console" };

	mocks::console_config consoleConfig{};
	mocks::participant_config participantConfig{};
	mocks::global_config globalConfig{};
	mocks::runtime runtime{};
	mocks::communication_cluster communicationCluster{};

	EXPECT_CALL(
		consoleConfig, get_enabled(
		)
	).WillRepeatedly(
		Return(true)
	);
	EXPECT_CALL(
		consoleConfig, get_slot(
		)
	).WillRepeatedly(
		Return(slot)
	);
	EXPECT_CALL(
		participantConfig, get_console(
		)
	).WillRepeatedly(
		[&]()->mocks::console_config const & {
			return consoleConfig;
		}
	);
	EXPECT_CALL(
		globalConfig, get_participant(
		)
	).WillRepeatedly(
		[&]()->mocks::participant_config const & {
			return participantConfig;
		}
	);
	EXPECT_CALL(
		runtime, schedule(
			_,
			_
		)
	);

	repositories::console<
		mocks::global_config,
		mocks::runtime,
		mocks::communication_cluster
	> target{
		partition,
		globalConfig,
		runtime,
		communicationCluster
	};

	auto receiver = target.find(
		partition,
		slot
	);
	auto end = target.end();

	ASSERT_THAT(
		receiver,
		Ne(::std::ref(end))
	);
};
TEST(ConsoleRepositoryTests, CachesReceiver) {
	string_t partition{ "test" };
	string_t slot{ "console" };

	mocks::console_config consoleConfig{};
	mocks::participant_config participantConfig{};
	mocks::global_config globalConfig{};
	mocks::runtime runtime{};
	mocks::communication_cluster communicationCluster{};

	EXPECT_CALL(
		consoleConfig, get_enabled(
		)
	).WillRepeatedly(
		Return(true)
	);
	EXPECT_CALL(
		consoleConfig, get_slot(
		)
	).WillRepeatedly(
		Return(slot)
	);
	EXPECT_CALL(
		participantConfig, get_console(
		)
	).WillRepeatedly(
		[&]()->mocks::console_config const & {
			return consoleConfig;
		}
	);
	EXPECT_CALL(
		globalConfig, get_participant(
		)
	).WillRepeatedly(
		[&]()->mocks::participant_config const & {
			return participantConfig;
		}
	);
	EXPECT_CALL(
		runtime, schedule(
			_,
			_
		)
	);

	repositories::console<
		mocks::global_config,
		mocks::runtime,
		mocks::communication_cluster
	> target{
		partition,
		globalConfig,
		runtime,
		communicationCluster
	};

	{
		auto receiver = target.find(
			partition,
			slot
		);
		auto cached = target.find(
			partition,
			slot
		);
		
		ASSERT_THAT(
			receiver,
			Eq(::std::ref(cached))
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
		Eq(::std::ref(end))
	);
};
TEST(ConsoleRepositoryTests, ControlledByConfig) {
	string_t partition{ "test" };
	string_t slot{ "console" };

	mocks::console_config consoleConfig{};
	mocks::participant_config participantConfig{};
	mocks::global_config globalConfig{};
	mocks::runtime runtime{};
	mocks::communication_cluster communicationCluster{};

	EXPECT_CALL(
		consoleConfig, get_enabled(
		)
	).WillRepeatedly(
		Return(false)
	);
	EXPECT_CALL(
		consoleConfig, get_slot(
		)
	).WillRepeatedly(
		Return(slot)
	);
	EXPECT_CALL(
		participantConfig, get_console(
		)
	).WillRepeatedly(
		[&]()->mocks::console_config const & {
			return consoleConfig;
		}
	);
	EXPECT_CALL(
		globalConfig, get_participant(
		)
	).WillRepeatedly(
		[&]()->mocks::participant_config const & {
			return participantConfig;
		}
	);

	repositories::console<
		mocks::global_config,
		mocks::runtime,
		mocks::communication_cluster
	> target{
		partition,
		globalConfig,
		runtime,
		communicationCluster
	};

	auto receiver = target.find(
		partition,
		slot
	);
	auto end = target.end();

	ASSERT_THAT(
		receiver,
		Eq(::std::ref(end))
	);
};
TEST(ConsoleRepositoryTests, ValidatesPartition) {
	string_t partition_a{ "test_a" };
	string_t partition_b{ "test_b" };
	string_t slot{ "console" };

	mocks::console_config consoleConfig{};
	mocks::participant_config participantConfig{};
	mocks::global_config globalConfig{};
	mocks::runtime runtime{};
	mocks::communication_cluster communicationCluster{};

	EXPECT_CALL(
		consoleConfig, get_enabled(
		)
	).WillRepeatedly(
		Return(true)
	);
	EXPECT_CALL(
		consoleConfig, get_slot(
		)
	).WillRepeatedly(
		Return(slot)
	);
	EXPECT_CALL(
		participantConfig, get_console(
		)
	).WillRepeatedly(
		[&]()->mocks::console_config const & {
			return consoleConfig;
		}
	);
	EXPECT_CALL(
		globalConfig, get_participant(
		)
	).WillRepeatedly(
		[&]()->mocks::participant_config const & {
			return participantConfig;
		}
	);

	repositories::console<
		mocks::global_config,
		mocks::runtime,
		mocks::communication_cluster
	> target{
		partition_a,
		globalConfig,
		runtime,
		communicationCluster
	};

	auto receiver = target.find(
		partition_b,
		slot
	);
	auto end = target.end();

	ASSERT_THAT(
		receiver,
		Eq(::std::ref(end))
	);
};
TEST(ConsoleRepositoryTests, ValidatesSlot) {
	string_t partition{ "test" };
	string_t slot_a{ "console_a" };
	string_t slot_b{ "console_b" };

	mocks::console_config consoleConfig{};
	mocks::participant_config participantConfig{};
	mocks::global_config globalConfig{};
	mocks::runtime runtime{};
	mocks::communication_cluster communicationCluster{};

	EXPECT_CALL(
		consoleConfig, get_enabled(
		)
	).WillRepeatedly(
		Return(true)
	);
	EXPECT_CALL(
		consoleConfig, get_slot(
		)
	).WillRepeatedly(
		Return(slot_a)
	);
	EXPECT_CALL(
		participantConfig, get_console(
		)
	).WillRepeatedly(
		[&]()->mocks::console_config const & {
			return consoleConfig;
		}
	);
	EXPECT_CALL(
		globalConfig, get_participant(
		)
	).WillRepeatedly(
		[&]()->mocks::participant_config const & {
			return participantConfig;
		}
	);

	repositories::console<
		mocks::global_config,
		mocks::runtime,
		mocks::communication_cluster
	> target{
		partition,
		globalConfig,
		runtime,
		communicationCluster
	};

	auto receiver = target.find(
		partition,
		slot_b
	);
	auto end = target.end();

	ASSERT_THAT(
		receiver,
		Eq(::std::ref(end))
	);
};

} // repositories
} // chat
} // ai

#endif
