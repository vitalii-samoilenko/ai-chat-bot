#ifndef AI_CHAT_THREADS_BACKED_TESTS_HPP
#define AI_CHAT_THREADS_BACKED_TESTS_HPP

#include "gtest/gtest.h"

#include "ai/chat/mocks/communication_cluster.hpp"
#include "ai/chat/mocks/media.hpp"

#include "ai/chat/model.hpp"
#include "ai/chat/threads/backed.hpp"

using namespace ::testing;

namespace ai {
namespace chat {
namespace threads {

TEST(BackedThreadTests, PushWritesToMedia) {
	string_t partition{ "test" };
	string_t slot{ "backed" };
	string_t content{ "Some content" };
	tag_t _tags[]{
		tag_t{ "Name A", "Value A" },
		tag_t{ "Name B", "Value B" }
	};
	tags_t tags{ _tags };

	mocks::CommunicationCluster communicationCluster{};

	backed<
		mocks::CommunicationCluster,
		mocks::Media
	> target{
		partition,
		slot,
		communicationCluster,
		[&](mocks::Media *that)->void {
			EXPECT_CALL(
				*that, insert_back(
					Gt(0),
					Eq(content),
					IsSupersetOf(tags)
				)
			).Times(
				Exactly(1)
			);
		}
	};

	target.push(
		content,
		tags
	);
};
TEST(BackedThreadTests, PushAppendsChannel) {
	string_t partition{ "test" };
	string_t slot{ "backed" };
	string_t content{ "Some content" };
	tags_t tags{};
	tag_t _appends[]{
		tag_t{ "channel.partition", partition },
		tag_t{ "channel.slot", slot }
	};
	tags_t appends{ _appends };

	mocks::CommunicationCluster communicationCluster{};

	backed<
		mocks::CommunicationCluster,
		mocks::Media
	> target{
		partition,
		slot,
		communicationCluster,
		[&](mocks::Media *that)->void {
			EXPECT_CALL(
				*that, insert_back(
					_,
					_,
					UnorderedElementsAreArray(appends)
				)
			).Times(
				Exactly(1)
			);
		}
	};

	target.push(
		content,
		tags
	);
};
TEST(BackedThreadTests, PushOverridesChannel) {
	string_t partition{ "test" };
	string_t slot{ "backed" };
	string_t content{ "Some content" };
	tag_t _tags[]{
		tag_t{ "channel.partition", "other_test" },
		tag_t{ "channel.slot", "other_backed" }
	};
	tags_t tags{ _tags };
	tag_t _overrides[]{
		tag_t{ "channel.partition", partition },
		tag_t{ "channel.slot", slot }
	};
	tags_t overrides{ _overrides };

	mocks::CommunicationCluster communicationCluster{};

	backed<
		mocks::CommunicationCluster,
		mocks::Media
	> target{
		partition,
		slot,
		communicationCluster,
		[&](mocks::Media *that)->void {
			EXPECT_CALL(
				*that, insert_back(
					_,
					_,
					UnorderedElementsAreArray(overrides)
				)
			).Times(
				Exactly(1)
			);
		}
	};

	target.push(
		content,
		tags
	);
};
TEST(BackedThreadTests, PushNotifiesReceivers) {
	string_t partition{ "test" };
	string_t slot{ "backed" };
	string_t content{ "Some content" };
	tag_t _tags[]{
		tag_t{ "Name A", "Value A" },
		tag_t{ "Name B", "Value B" }
	};
	tags_t tags{ _tags };
	timepoint_t timestamp{ -1 };
	message_t _message{
		timestamp,
		content,
		tags
	};
	message_t *message{ &_message };
	string_t rpartition_a{ "rtest_a" };
	string_t rslot_a{ "mock_a" };
	string_t rpartition_b{ "rtest_b" };
	string_t rslot_b{ "mock_b" };
	string_t rpartition_c{ "rtest_c" };
	string_t rslot_c{ "mock_c" };

	mocks::CommunicationCluster communicationCluster{};

	EXPECT_CALL(
		communicationCluster, notify(
			Eq(rpartition_a),
			Eq(rslot_a),
			Eq(get_timestamp(*message)),
			Eq(get_content(*message)),
			ElementsAreArray(get_tags(*message))
		)
	).Times(
		Exactly(1)
	);
	EXPECT_CALL(
		communicationCluster, notify(
			Eq(rpartition_b),
			Eq(rslot_b),
			Eq(get_timestamp(*message)),
			Eq(get_content(*message)),
			ElementsAreArray(get_tags(*message))
		)
	).Times(
		Exactly(0)
	);
	EXPECT_CALL(
		communicationCluster, notify(
			Eq(rpartition_c),
			Eq(rslot_c),
			Eq(get_timestamp(*message)),
			Eq(get_content(*message)),
			ElementsAreArray(get_tags(*message))
		)
	).Times(
		Exactly(1)
	);

	backed<
		mocks::CommunicationCluster,
		mocks::Media
	> target{
		partition,
		slot,
		communicationCluster,
		[&](mocks::Media *that)->void {
			EXPECT_CALL(
				*that, insert_back(
					_,
					_,
					_
				)
			).WillRepeatedly(
				Return(message)
			);
		}
	};

	target.accept(
		rpartition_a,
		rslot_a
	);
	target.accept(
		rpartition_b,
		rslot_b
	);
	target.accept(
		rpartition_c,
		rslot_c
	);
	target.dismiss(
		rpartition_b,
		rslot_b
	);
	target.push(
		content,
		tags
	);
};
TEST(BackedThreadTests, PushIsResilient) {
	string_t partition{ "test" };
	string_t slot{ "backed" };
	string_t content{ "Some content" };
	tags_t tags{};
	timepoint_t timestamp{ -1 };
	message_t _message{
		timestamp,
		content,
		tags
	};
	message_t *message{ &_message };
	string_t rpartition_a{ "rtest_a" };
	string_t rslot_a{ "mock_a" };
	string_t rpartition_b{ "rtest_b" };
	string_t rslot_b{ "mock_b" };

	mocks::CommunicationCluster communicationCluster{};

	EXPECT_CALL(
		communicationCluster, notify(
			Eq(rpartition_a),
			Eq(rslot_a),
			_,
			_,
			_
		)
	).Times(
		Exactly(1)
	).WillOnce(
		Throw("error_a")
	);
	EXPECT_CALL(
		communicationCluster, notify(
			Eq(rpartition_b),
			Eq(rslot_b),
			_,
			_,
			_
		)
	).Times(
		Exactly(1)
	).WillOnce(
		Throw("error_b")
	);

	backed<
		mocks::CommunicationCluster,
		mocks::Media
	> target{
		partition,
		slot,
		communicationCluster,
		[&](mocks::Media *that)->void {
			EXPECT_CALL(
				*that, insert_back(
					_,
					_,
					_
				)
			).WillRepeatedly(
				Return(message)
			);
		}
	};

	target.accept(
		rpartition_a,
		rslot_a
	);
	target.accept(
		rpartition_b,
		rslot_b
	);
	target.push(
		content,
		tags
	);
};

} // threads
} // chat
} // ai

#endif
