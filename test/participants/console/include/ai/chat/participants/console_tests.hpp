#ifndef AI_CHAT_PARTICIPANTS_CONSOLE_TESTS_HPP
#define AI_CHAT_PARTICIPANTS_CONSOLE_TESTS_HPP

#include "gtest/gtest.h"

#include "ai/chat/mocks/communication_cluster.hpp"

#include "ai/chat/model.hpp"
#include "ai/chat/participants/console.hpp"

using namespace ::testing;

namespace ai {
namespace chat {
namespace participants {

TEST(ConsoleParticipantTests, AsyncUnitOfWork) {
	string_t partition{};
	string_t slot{};

	mocks::CommunicationCluster communicationCluster{};

	console<
		mocks::CommunicationCluster
	> target{
		partition,
		slot,
		communicationCluster
	};

	target();
};
TEST(ConsoleParticipantTests, PushesToInput) {
	string_t partition{ "test" };
	string_t slot{ "console" };
	string_t cpartition{ "ctest" };
	string_t cslot{ "mock" };
	string_t line{ "Hello, World!\n" };
	string_t content{ line.substr(0, line.length() - 1) };
	tag_t _tags[]{
		tag_t{ "producer.partition", partition },
		tag_t{ "producer.slot", slot }
	};
	tags_t tags{ _tags };

	mocks::CommunicationCluster communicationCluster{};

	EXPECT_CALL(
		communicationCluster, push(
			Eq(cpartition),
			Eq(cslot),
			Eq(content),
			ElementsAreArray(tags)
		)
	).Times(
		Exactly(1)
	);

#ifdef __unix__
	int io_pipe[2];
	ASSERT_THAT(
		pipe(io_pipe),
		Ne(-1)
	);
	int temp{ dup2(io_pipe[0], STDIN_FILENO) };
	ASSERT_THAT(
		temp,
		Ne(-1)
	);
	ASSERT_THAT(
		write(io_pipe[1],
			line.data(), sizeof(char) * line.length()
		),
		Ne(-1)
	);
#endif

	console<
		mocks::CommunicationCluster
	> target{
		partition,
		slot,
		communicationCluster
	};

	target.join(
		cpartition,
		cslot
	);
	target();

#ifdef __unix__
	ASSERT_THAT(
		dup2(temp, STDIN_FILENO),
		Ne(-1)
	);
#endif
};

} // participants
} // chat
} // ai

#endif
