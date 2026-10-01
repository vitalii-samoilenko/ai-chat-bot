#ifndef AI_CHAT_THREADS_MEDIA_NULL_TESTS_HPP
#define AI_CHAT_THREADS_MEDIA_NULL_TESTS_HPP

#include <span>
#include <string>
#include <string_view>
#include <tuple>
#include <vector>

#include "ai/chat/threads/media/null.hpp"

namespace ai {
namespace chat {
namespace threads {
namespace media {

TEST(NullTests, InsertBackReturnsValidIterator) {
	::std::string content{ "Some content" };
	::std::string tag_name_a{ "Some name A" };
	::std::string tag_value_a{ "Some value A" };
	::std::string tag_name_b{ "Some name B" };
	::std::string tag_value_b{ "Some value B" };
	::std::vector<
		::std::tuple<
			::std::string_view,
			::std::string_view
		>
	> _tags{
		::std::make_tuple(
			::std::string_view{ tag_name_a },
			::std::string_view{ tag_value_a}
		),
		::std::make_tuple(
			::std::string_view{ tag_name_b },
			::std::string_view{ tag_value_b }
		)
	};

	null instance{};

	auto message = instance.insert_back(
		content,
		_tags
	);

	ASSERT_GT(::std::get<0>(*message), 0);
	ASSERT_EQ(::std::get<1>(*message), content);
	auto tags = ::std::get<2>(*message);
	ASSERT_EQ(tags.size(), 2);
	ASSERT_EQ(::std::get<0>(tags[0]), tag_name_a);
	ASSERT_EQ(::std::get<1>(tags[0]), tag_value_a);
	ASSERT_EQ(::std::get<0>(tags[1]), tag_name_b);
	ASSERT_EQ(::std::get<1>(tags[1]), tag_value_b);
};

} // media
} // threads
} // chat
} // ai

#endif
