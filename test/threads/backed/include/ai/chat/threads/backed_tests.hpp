#ifndef AI_CHAT_THREADS_BACKED_TESTS_HPP
#define AI_CHAT_THREADS_BACKED_TESTS_HPP

#include <vector>

#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include "ai/chat/threads/backed.hpp"

using namespace ::testing;

namespace ai {
namespace chat {
namespace threads {

class ParticipantClusterMock {
public:
	MOCK_METHOD(
		void,
		notify, (
			::std::string_view,
			::std::string_view,
			long long,
			::std::string_view,
			(::std::span<
				::std::tuple<
					::std::string_view,
					::std::string_view
				>
			>)
		),
		(const)
	);
};

class MediaMock {
public:
	using value_type = ::std::tuple<
		long long,
		::std::string_view,
		::std::span<
			::std::tuple<
				::std::string_view,
				::std::string_view
			>
		>
	>;
	using iterator = value_type *;

	template<
		typename Func
	> MediaMock(Func &&configure) {
		configure(this);
	};

	MOCK_METHOD(
		value_type *,
		insert_back,
		(
			::std::string_view,
			(::std::span<
				::std::tuple<
					::std::string_view,
					::std::string_view
				>
			>)
		)
	);
};

TEST(BackedThreadTests, PushWritesToMedia) {
	::std::string_view partition{ "test" };
	::std::string_view name{ "backed" };
	::std::string_view content{ "Some content" };
	::std::string_view tag_name_a{ "Name A" };
	::std::string_view tag_value_a{ "Value A" };
	::std::string_view tag_name_b{ "Name B" };
	::std::string_view tag_value_b{ "Value B" };
	::std::vector<
		::std::tuple<
			::std::string_view,
			::std::string_view
		>
	> tags{
		::std::make_tuple(
			tag_name_a,
			tag_value_a
		),
		::std::make_tuple(
			tag_name_b,
			tag_value_b
		)
	};

	StrictMock<
		ParticipantClusterMock
	> participantCluster{};

	backed<
		StrictMock<
			ParticipantClusterMock
		>,
		StrictMock<
			MediaMock
		>
	> instance{
		partition,
		name,
		participantCluster,
		[&](MediaMock *that)->void {
			EXPECT_CALL(
				*that,
				insert_back(
					Eq(content),
					IsSupersetOf(
						tags.begin(),
						tags.end()
					)
				)
			).Times(
				Exactly(1)
			).WillOnce(
				Return(nullptr)
			);
		}
	};

	instance.push(
		content,
		tags
	);
};
TEST(BackedThreadTests, PushAppendsChannel) {
	::std::string_view partition{ "test" };
	::std::string_view name{ "backed" };
	::std::string_view content{ "Some content" };
	::std::vector<
		::std::tuple<
			::std::string_view,
			::std::string_view
		>
	> tags{};
	::std::string_view tag_name_cpartition{ "channel.partition" };
	::std::string_view tag_name_cname{ "channel.name" };
	::std::vector<
		::std::tuple<
			::std::string_view,
			::std::string_view
		>
	> appends{
		::std::make_tuple(
			tag_name_cpartition,
			partition
		),
		::std::make_tuple(
			tag_name_cname,
			name
		)
	};

	StrictMock<
		ParticipantClusterMock
	> participantCluster{};

	backed<
		StrictMock<
			ParticipantClusterMock
		>,
		StrictMock<
			MediaMock
		>
	> instance{
		partition,
		name,
		participantCluster,
		[&](MediaMock *that)->void {
			EXPECT_CALL(
				*that,
				insert_back(
					_,
					UnorderedElementsAreArray(
						appends.begin(),
						appends.end()
					)
				)
			).Times(
				Exactly(1)
			).WillOnce(
				Return(nullptr)
			);
		}
	};

	instance.push(
		content,
		tags
	);
};
TEST(BackedThreadTests, PushOverridesChannel) {
	::std::string_view partition{ "test" };
	::std::string_view name{ "backed" };
	::std::string_view content{ "Some content" };
	::std::string_view tag_name_cpartition{ "channel.partition" };
	::std::string_view tag_value_cpartition{ "other_test" };
	::std::string_view tag_name_cname{ "channel.name" };
	::std::string_view tag_value_cname{ "other_backed" };
	::std::vector<
		::std::tuple<
			::std::string_view,
			::std::string_view
		>
	> tags{
		::std::make_tuple(
			tag_name_cpartition,
			tag_value_cpartition
		),
		::std::make_tuple(
			tag_name_cname,
			tag_value_cpartition
		)
	};
	::std::vector<
		::std::tuple<
			::std::string_view,
			::std::string_view
		>
	> overrides{
		::std::make_tuple(
			tag_name_cpartition,
			partition
		),
		::std::make_tuple(
			tag_name_cname,
			name
		)
	};

	StrictMock<
		ParticipantClusterMock
	> participantCluster{};

	backed<
		StrictMock<
			ParticipantClusterMock
		>,
		StrictMock<
			MediaMock
		>
	> instance{
		partition,
		name,
		participantCluster,
		[&](MediaMock *that)->void {
			EXPECT_CALL(
				*that,
				insert_back(
					_,
					UnorderedElementsAreArray(
						overrides.begin(),
						overrides.end()
					)
				)
			).Times(
				Exactly(1)
			).WillOnce(
				Return(nullptr)
			);
		}
	};

	instance.push(
		content,
		tags
	);
};
TEST(BackedThreadTests, PushNotifiesReceivers) {
	::std::string_view partition{ "test" };
	::std::string_view name{ "backed" };
	::std::string_view content{ "Some content" };
	::std::string_view tag_name_a{ "Name A" };
	::std::string_view tag_value_a{ "Value A" };
	::std::string_view tag_name_b{ "Name B" };
	::std::string_view tag_value_b{ "Value B" };
	::std::vector<
		::std::tuple<
			::std::string_view,
			::std::string_view
		>
	> tags{
		::std::make_tuple(
			tag_name_a,
			tag_value_a
		),
		::std::make_tuple(
			tag_name_b,
			tag_value_b
		)
	};
	long long timestamp{ 10 };
	::std::tuple<
		long long,
		::std::string_view,
		::std::span<
			::std::tuple<
				::std::string_view,
				::std::string_view
			>
		>
	> message{
		timestamp,
		content,
		tags
	};
	::std::string_view rpartition{ "receivers" };
	::std::string_view rname_a{ "some a" };
	::std::string_view rname_b{ "some b" };
	StrictMock<
		ParticipantClusterMock
	> participantCluster{};
	EXPECT_CALL(
		participantCluster,
		notify(
			Eq(rpartition),
			Eq(rname_a),
			Eq(timestamp),
			Eq(content),
			UnorderedElementsAreArray(
				tags.begin(),
				tags.end()
			)
		)
	).Times(
		Exactly(1)
	);

	backed<
		StrictMock<
			ParticipantClusterMock
		>,
		StrictMock<
			MediaMock
		>
	> instance{
		partition,
		name,
		participantCluster,
		[&](MediaMock *that)->void {
			EXPECT_CALL(
				*that,
				insert_back(
					_,
					_
				)
			).Times(
				Exactly(1)
			).WillOnce(
				Return(&message)
			);
		}
	};

	instance.accept(
		rpartition,
		rname_a
	);
	instance.accept(
		rpartition,
		rname_b
	);
	instance.dismiss(
		rpartition,
		rname_b
	);
	instance.push(
		content,
		tags
	);
};

} // threads
} // chat
} // ai

#endif
