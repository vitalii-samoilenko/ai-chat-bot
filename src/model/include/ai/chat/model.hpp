#ifndef AI_CHAT_MODEL_HPP
#define AI_CHAT_MODEL_HPP

#include <span>
#include <string_view>
#include <tuple>
#include <utility>

namespace ai {
namespace chat {

using timepoint_t = long long;
using string_t = ::std::string_view;
using tag_t = ::std::pair<
	string_t,
	string_t
>;
using tags_t = ::std::span<
	tag_t
>;
using message_t = ::std::tuple<
	timepoint_t,
	string_t,
	tags_t
>;

timepoint_t
&get_timestamp(
	message_t &message
) {
	return ::std::get<0>(
		message
	);
};
string_t
&get_content(
	message_t &message
) {
	return ::std::get<1>(
		message
	);
};
tags_t
&get_tags(
	message_t &message
) {
	return ::std::get<2>(
		message
	);
};
string_t
&get_name(
	tag_t &tag
) {
	return ::std::get<0>(
		tag
	);
};
string_t
&get_value(
	tag_t &tag
) {
	return ::std::get<1>(
		tag
	);
};

timepoint_t const
&get_timestamp(
	message_t const &message
) {
	return ::std::get<0>(
		message
	);
};
string_t const
&get_content(
	message_t const &message
) {
	return ::std::get<1>(
		message
	);
};
tags_t const
&get_tags(
	message_t const &message
) {
	return ::std::get<2>(
		message
	);
};
string_t const
&get_name(
	tag_t const &tag
) {
	return ::std::get<0>(
		tag
	);
};
string_t const
&get_value(
	tag_t const &tag
) {
	return ::std::get<1>(
		tag
	);
};

} // chat
} // ai

#endif
