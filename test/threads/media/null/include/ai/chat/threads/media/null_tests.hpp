#ifndef AI_CHAT_THREADS_MEDIA_NULL_TESTS_HPP
#define AI_CHAT_THREADS_MEDIA_NULL_TESTS_HPP

#include <functional>

#include "ai/chat/model.hpp"
#include "ai/chat/threads/media/null.hpp"

using namespace ::testing;

namespace ai {
namespace chat {
namespace threads {
namespace media {

TEST(NullMediaTests, EmptyRange) {
	null target{};

	auto begin = target.begin();
	auto end = target.end();

	ASSERT_THAT(
		begin,
		Eq(::std::ref(end))
	);
};
TEST(NullMediaTests, InsertBackImmediatelyDiscards) {
	timepoint_t timestamp{};
	string_t content{};
	tags_t tags{};

	null target{};

	auto message = target.insert_back(
		timestamp,
		content,
		tags
	);

	auto begin = target.begin();
	auto end = target.end();

	ASSERT_THAT(
		begin,
		Eq(::std::ref(end))
	);
};
TEST(NullMediaTests, InsertBackIsTemporary) {
	timepoint_t timestamp{};
	string_t content{};
	tags_t tags{};

	null target{};

	auto message = target.insert_back(
		timestamp,
		content,
		tags
	);

	auto end = target.end();

	ASSERT_THAT(
		message,
		Ne(::std::ref(end))
	);
	ASSERT_THAT(
		message,
		Lt(::std::ref(end))
	);

	++message;

	ASSERT_THAT(
		message,
		Eq(::std::ref(end))
	);
};
TEST(NullMediaTests, InsertBackIsMeaningful) {
	timepoint_t timestamp{ -1 };
	string_t content{ "Some content" };
	tag_t _tags[]{
		tag_t{ "Name A", "Value A" },
		tag_t{ "Name B", "Value B" }
	};
	tags_t tags{ _tags };

	null target{};

	auto message = target.insert_back(
		timestamp,
		content,
		tags
	);

	ASSERT_THAT(
		get_timestamp(*message),
		Eq(timestamp)
	);
	ASSERT_THAT(
		get_content(*message),
		Eq(content)
	);
	ASSERT_THAT(
		get_tags(*message),
		ElementsAreArray(tags)
	);
};

} // media
} // threads
} // chat
} // ai

#endif
