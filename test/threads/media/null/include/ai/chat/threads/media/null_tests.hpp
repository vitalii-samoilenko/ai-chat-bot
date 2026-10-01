#ifndef AI_CHAT_THREADS_MEDIA_NULL_TESTS_HPP
#define AI_CHAT_THREADS_MEDIA_NULL_TESTS_HPP

#include <functional>
#include <vector>

#include "ai/chat/threads/media/null.hpp"

using namespace ::testing;

namespace ai {
namespace chat {
namespace threads {
namespace media {

TEST(NullMediaTests, EmptyRange) {
	null instance{};

	auto begin = instance.begin();
	auto end = instance.end();

	ASSERT_THAT(
		begin,
		Eq(::std::ref(end))
	);
};
TEST(NullMediaTests, InsertBackImmediatelyDiscards) {
	::std::string content{};
	::std::vector<
		::std::tuple<
			::std::string_view,
			::std::string_view
		>
	> tags{};

	null instance{};

	auto message = instance.insert_back(
		content,
		tags
	);

	auto begin = instance.begin();
	auto end = instance.end();

	ASSERT_THAT(
		begin,
		Eq(::std::ref(end))
	);
};
TEST(NullMediaTests, InsertBackIsTemporary) {
	::std::string content{};
	::std::vector<
		::std::tuple<
			::std::string_view,
			::std::string_view
		>
	> tags{};

	null instance{};

	auto message = instance.insert_back(
		content,
		tags
	);

	auto end = instance.end();

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
	::std::string_view content{ "Some content" };
	::std::string_view tag_name_a{ "Some name A" };
	::std::string_view tag_value_a{ "Some value A" };
	::std::string_view tag_name_b{ "Some name B" };
	::std::string_view tag_value_b{ "Some value B" };
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

	null instance{};

	auto message = instance.insert_back(
		content,
		tags
	);

	ASSERT_THAT(
		*message,
		FieldsAre(
			Gt(0),
			Eq(content),
			ElementsAreArray(
				tags.begin(),
				tags.end()
			)
		)
	);
};

} // media
} // threads
} // chat
} // ai

#endif
