#ifndef AI_CHAT_PARTICIPANTS_CONSOLE_TESTS_HPP
#define AI_CHAT_PARTICIPANTS_CONSOLE_TESTS_HPP

#include <string_view>

#include "gtest/gtest.h"
#include "re2/re2.h"

#include "ai/chat/mocks/communication_cluster.hpp"
#include "ai/chat/stubs/console.hpp"

#include "ai/chat/model.hpp"
#include "ai/chat/participants/console.hpp"

using namespace ::testing;

namespace ai {
namespace chat {
namespace participants {

TEST(ConsoleParticipantTests, AsyncUnitOfWork) {
	string_t partition{};
	string_t slot{};

	mocks::communication_cluster communicationCluster{};

	console<
		mocks::communication_cluster
	> target{
		partition,
		slot,
		communicationCluster
	};

	target();
};
TEST(ConsoleParticipantTests, PushesInput) {
	string_t partition{ "test" };
	string_t slot{ "console" };
	string_t ipartition{ "itest" };
	string_t islot{ "imock" };
	::std::string_view line{ "Hello, World!\n" };
	string_t content{ line.substr(0, line.length() - 1) };
	tag_t _tags[]{
		tag_t{ "producer.partition", partition },
		tag_t{ "producer.slot", slot }
	};
	tags_t tags{ _tags };

	mocks::communication_cluster communicationCluster{};
	stubs::console shell{};

	EXPECT_CALL(
		communicationCluster, push(
			Eq(ipartition),
			Eq(islot),
			Eq(content),
			ElementsAreArray(tags)
		)
	).Times(
		Exactly(1)
	);

	console<
		mocks::communication_cluster
	> target{
		partition,
		slot,
		communicationCluster
	};

	target.join(
		ipartition,
		islot
	);

	shell.write(line);
	target();
};
TEST(ConsoleParticipantTests, DetectsReceivers) {
	string_t partition{};
	string_t slot{};
	string_t ipartition{ "itest" };
	string_t islot{ "imock" };
	::std::string_view line{ "Hello, @some1 and @some2!\n" };
	tag_t _tags[]{
		tag_t{ "receiver.slot", "some1" },
		tag_t{ "receiver.slot", "some2" }
	};
	tags_t tags{ _tags };

	mocks::communication_cluster communicationCluster{};
	stubs::console shell{};

	EXPECT_CALL(
		communicationCluster, push(
			Eq(ipartition),
			Eq(islot),
			_,
			IsSupersetOf(tags)
		)
	).Times(
		Exactly(1)
	);

	console<
		mocks::communication_cluster
	> target{
		partition,
		slot,
		communicationCluster
	};

	target.join(
		ipartition,
		islot
	);

	shell.write(line);
	target();
};
TEST(ConsoleParticipantTests, SingleChannelInput) {
	string_t partition{};
	string_t slot{};
	string_t ipartition_a{ "itest_a" };
	string_t islot_a{ "imock_a" };
	string_t ipartition_b{ "itest_b" };
	string_t islot_b{ "imock_b" };
	::std::string_view line{ "Hello, World!\n" };
	tags_t tags{};

	mocks::communication_cluster communicationCluster{};
	stubs::console shell{};

	EXPECT_CALL(
		communicationCluster, push(
			Eq(ipartition_a),
			Eq(islot_a),
			_,
			_
		)
	).Times(
		Exactly(0)
	);
	EXPECT_CALL(
		communicationCluster, push(
			Eq(ipartition_b),
			Eq(islot_b),
			_,
			_
		)
	).Times(
		Exactly(1)
	);

	console<
		mocks::communication_cluster
	> target{
		partition,
		slot,
		communicationCluster
	};

	target.join(
		ipartition_a,
		islot_a
	);
	target.join(
		ipartition_b,
		islot_b
	);

	shell.write(line);
	target();
};
TEST(ConsoleParticipantTests, PushesCommand) {
	string_t partition{ "test" };
	string_t slot{ "console" };
	string_t cpartition{ "command" };
	string_t cslot{ "cmock" };
	::std::string_view line{ "~Just Do It!\n" };
	string_t content{ line.substr(1, line.length() - 2) };
	tag_t _tags[]{
		tag_t{ "producer.partition", partition },
		tag_t{ "producer.slot", slot }
	};
	tags_t tags{ _tags };

	mocks::communication_cluster communicationCluster{};
	stubs::console shell{};

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

	console<
		mocks::communication_cluster
	> target{
		partition,
		slot,
		communicationCluster
	};

	target.join(
		cpartition,
		cslot
	);

	shell.write(line);
	target();
};
TEST(ConsoleParticipantTests, SingleChannelCommand) {
	string_t partition{};
	string_t slot{};
	string_t cpartition{ "command" };
	string_t cslot_a{ "cmock_a" };
	string_t cslot_b{ "cmock_b" };
	::std::string_view line{ "~Just Do It!\n" };
	tags_t tags{};

	mocks::communication_cluster communicationCluster{};
	stubs::console shell{};

	EXPECT_CALL(
		communicationCluster, push(
			Eq(cpartition),
			Eq(cslot_a),
			_,
			_
		)
	).Times(
		Exactly(0)
	);
	EXPECT_CALL(
		communicationCluster, push(
			Eq(cpartition),
			Eq(cslot_b),
			_,
			_
		)
	).Times(
		Exactly(1)
	);

	console<
		mocks::communication_cluster
	> target{
		partition,
		slot,
		communicationCluster
	};

	target.join(
		cpartition,
		cslot_a
	);
	target.join(
		cpartition,
		cslot_b
	);

	shell.write(line);
	target();
};
TEST(ConsoleParticipantTests, InputDoesNotLeak) {
	string_t partition{};
	string_t slot{};
	string_t cpartition{ "command" };
	string_t cslot{ "cmock" };
	string_t ipartition{ "itest" };
	string_t islot{ "imock" };
	::std::string_view line{ "Hello, World!\n" };
	tags_t tags{};

	mocks::communication_cluster communicationCluster{};
	stubs::console shell{};

	EXPECT_CALL(
		communicationCluster, push(
			Eq(cpartition),
			Eq(cslot),
			_,
			_
		)
	).Times(
		Exactly(0)
	);
	EXPECT_CALL(
		communicationCluster, push(
			Eq(ipartition),
			Eq(islot),
			_,
			_
		)
	).Times(
		Exactly(1)
	);

	console<
		mocks::communication_cluster
	> target{
		partition,
		slot,
		communicationCluster
	};

	target.join(
		cpartition,
		cslot
	);
	target.join(
		ipartition,
		islot
	);

	shell.write(line);
	target();
};
TEST(ConsoleParticipantTests, CommandDoesNotLeak) {
	string_t partition{};
	string_t slot{};
	string_t cpartition{ "command" };
	string_t cslot{ "cmock" };
	string_t ipartition{ "itest" };
	string_t islot{ "imock" };
	::std::string_view line{ "~Just Do It!\n" };
	tags_t tags{};

	mocks::communication_cluster communicationCluster{};
	stubs::console shell{};

	EXPECT_CALL(
		communicationCluster, push(
			Eq(cpartition),
			Eq(cslot),
			_,
			_
		)
	).Times(
		Exactly(1)
	);
	EXPECT_CALL(
		communicationCluster, push(
			Eq(ipartition),
			Eq(islot),
			_,
			_
		)
	).Times(
		Exactly(0)
	);

	console<
		mocks::communication_cluster
	> target{
		partition,
		slot,
		communicationCluster
	};

	target.join(
		cpartition,
		cslot
	);
	target.join(
		ipartition,
		islot
	);

	shell.write(line);
	target();
};
TEST(ConsoleParticipantTests, PushIsResilient) {
	string_t partition{};
	string_t slot{};
	string_t ipartition{ "itest" };
	string_t islot{ "imock" };
	::std::string_view line{ "Hello, World!\n" };
	tags_t tags{};

	mocks::communication_cluster communicationCluster{};
	stubs::console shell{};

	EXPECT_CALL(
		communicationCluster, push(
			Eq(ipartition),
			Eq(islot),
			_,
			_
		)
	).WillRepeatedly(
		Throw("some error")
	);

	console<
		mocks::communication_cluster
	> target{
		partition,
		slot,
		communicationCluster
	};

	target.join(
		ipartition,
		islot
	);

	shell.write(line);
	target();
};
TEST(ConsoleParticipantTests, LeavesRecentOnly) {
	string_t partition{};
	string_t slot{};
	string_t ipartition_a{ "itest_a" };
	string_t islot_a{ "imock_a" };
	string_t ipartition_b{ "itest_b" };
	string_t islot_b{ "imock_b" };
	::std::string_view line{ "Hello, World!\n" };
	tags_t tags{};

	mocks::communication_cluster communicationCluster{};
	stubs::console shell{};

	EXPECT_CALL(
		communicationCluster, push(
			Eq(ipartition_a),
			Eq(islot_a),
			_,
			_
		)
	).Times(
		Exactly(0)
	);
	EXPECT_CALL(
		communicationCluster, push(
			Eq(ipartition_b),
			Eq(islot_b),
			_,
			_
		)
	).Times(
		Exactly(1)
	);

	console<
		mocks::communication_cluster
	> target{
		partition,
		slot,
		communicationCluster
	};

	target.join(
		ipartition_a,
		islot_a
	);
	target.join(
		ipartition_b,
		islot_b
	);
	target.leave(
		ipartition_a,
		islot_a
	);

	shell.write(line);
	target();

	target.leave(
		ipartition_b,
		islot_b
	);

	shell.write(line);
	target();
};
TEST(ConsoleParticipantTests, NotifiesInputDelivery) {
	string_t partition{ "test" };
	string_t slot{ "console" };
	timepoint_t timestamp{};
	string_t content{ "Hello, World!" };
	tag_t _tags[]{
		tag_t{ "channel.partition", "topic" },
		tag_t{ "producer.partition", partition },
		tag_t{ "producer.slot", slot }
	};
	tags_t tags{ _tags };

	mocks::communication_cluster communicationCluster{};
	stubs::console shell{};

	console<
		mocks::communication_cluster
	> target{
		partition,
		slot,
		communicationCluster
	};

	target.notify(
		timestamp,
		content,
		tags
	);

	::std::string_view line{ shell.read() };
	ASSERT_THAT(
		::RE2::PartialMatch(
			line,
			slot
		),
		IsTrue()
	);
	ASSERT_THAT(
		::RE2::PartialMatch(
			line,
			content
		),
		IsTrue()
	);
};
TEST(ConsoleParticipantTests, NotifiesCommandDelivery) {
	string_t partition{ "test" };
	string_t slot{ "console" };
	timepoint_t timestamp{};
	string_t content{ "Hello, World!" };
	tag_t _tags[]{
		tag_t{ "channel.partition", "command" },
		tag_t{ "producer.partition", partition },
		tag_t{ "producer.slot", slot }
	};
	tags_t tags{ _tags };

	mocks::communication_cluster communicationCluster{};
	stubs::console shell{};

	console<
		mocks::communication_cluster
	> target{
		partition,
		slot,
		communicationCluster
	};

	target.notify(
		timestamp,
		content,
		tags
	);

	::std::string_view line{ shell.read() };
	ASSERT_THAT(
		::RE2::PartialMatch(
			line,
			slot
		),
		IsTrue()
	);
	ASSERT_THAT(
		::RE2::PartialMatch(
			line,
			content
		),
		IsTrue()
	);
};
TEST(ConsoleParticipantTests, NotifiesInput) {
	string_t partition{};
	string_t slot{};
	string_t ppartition{ "test" };
	string_t pslot{ "data" };
	timepoint_t timestamp{};
	string_t content{ "Hello, World!" };
	tag_t _tags[]{
		tag_t{ "channel.partition", "topic" },
		tag_t{ "producer.partition", ppartition },
		tag_t{ "producer.slot", pslot }
	};
	tags_t tags{ _tags };

	mocks::communication_cluster communicationCluster{};
	stubs::console shell{};

	console<
		mocks::communication_cluster
	> target{
		partition,
		slot,
		communicationCluster
	};

	target.notify(
		timestamp,
		content,
		tags
	);

	::std::string_view line{ shell.read() };
	ASSERT_THAT(
		::RE2::PartialMatch(
			line,
			pslot
		),
		IsTrue()
	);
	ASSERT_THAT(
		::RE2::PartialMatch(
			line,
			content
		),
		IsTrue()
	);
};
TEST(ConsoleParticipantTests, NotifiesReject) {
	string_t partition{};
	string_t slot{};
	timepoint_t timestamp{};
	string_t content{};
	tag_t _tags[]{
		tag_t{ "channel.partition", "reject" }
	};
	tags_t tags{ _tags };

	mocks::communication_cluster communicationCluster{};
	stubs::console shell{};

	console<
		mocks::communication_cluster
	> target{
		partition,
		slot,
		communicationCluster
	};

	target.notify(
		timestamp,
		content,
		tags
	);

	ASSERT_THAT(
		::RE2::PartialMatch(
			shell.read(),
			"Message is rejected"
		),
		IsTrue()
	);
};
TEST(ConsoleParticipantTests, NotifiesReview) {
	string_t partition{};
	string_t slot{};
	timepoint_t timestamp{};
	string_t content{};
	tag_t _tags[]{
		tag_t{ "channel.partition", "review" }
	};
	tags_t tags{ _tags };

	mocks::communication_cluster communicationCluster{};
	stubs::console shell{};

	console<
		mocks::communication_cluster
	> target{
		partition,
		slot,
		communicationCluster
	};

	target.notify(
		timestamp,
		content,
		tags
	);

	ASSERT_THAT(
		::RE2::PartialMatch(
			shell.read(),
			"Message is in review"
		),
		IsTrue()
	);
};
TEST(ConsoleParticipantTests, NotifiesUnauthorize) {
	string_t partition{};
	string_t slot{};
	timepoint_t timestamp{};
	string_t content{};
	tag_t _tags[]{
		tag_t{ "channel.partition", "unauthorize" }
	};
	tags_t tags{ _tags };

	mocks::communication_cluster communicationCluster{};
	stubs::console shell{};

	console<
		mocks::communication_cluster
	> target{
		partition,
		slot,
		communicationCluster
	};

	target.notify(
		timestamp,
		content,
		tags
	);

	ASSERT_THAT(
		::RE2::PartialMatch(
			shell.read(),
			"Command not authorized"
		),
		IsTrue()
	);
};
TEST(ConsoleParticipantTests, NotifiesError) {
	string_t partition{};
	string_t slot{};
	timepoint_t timestamp{};
	string_t content{};
	tag_t _tags[]{
		tag_t{ "channel.partition", "error" }
	};
	tags_t tags{ _tags };

	mocks::communication_cluster communicationCluster{};
	stubs::console shell{};

	console<
		mocks::communication_cluster
	> target{
		partition,
		slot,
		communicationCluster
	};

	target.notify(
		timestamp,
		content,
		tags
	);

	ASSERT_THAT(
		::RE2::PartialMatch(
			shell.read(),
			"Command failed"
		),
		IsTrue()
	);
};

} // participants
} // chat
} // ai

#endif
