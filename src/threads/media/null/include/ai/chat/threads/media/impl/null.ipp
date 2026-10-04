#ifndef AI_CHAT_THREADS_MEDIA_IMPL_NULL_IPP
#define AI_CHAT_THREADS_MEDIA_IMPL_NULL_IPP

#include "ai/chat/threads/media/null.hpp"

::ai::chat::threads::media::null::iterator::iterator(
	::ai::chat::timepoint_t timestamp,
	::ai::chat::string_t content,
	::ai::chat::tags_t tags
) : _that{
		timestamp,
		content,
		tags
	} {

};

bool
::ai::chat::threads::media::null::iterator::operator==(
	::ai::chat::threads::media::null::iterator const &other
) const {
	return get_timestamp(_that) == get_timestamp(other._that);
};
bool
::ai::chat::threads::media::null::iterator::operator<(
	::ai::chat::threads::media::null::iterator const &other
) const {
	return get_timestamp(_that) < get_timestamp(other._that);
};
::ai::chat::threads::media::null::iterator
&::ai::chat::threads::media::null::iterator::operator++(
) {
	get_timestamp(_that) = TheEndTimes;
	get_content(_that) = string_t{};
	get_tags(_that) = tags_t{};
	return *this;
};
::ai::chat::message_t
&::ai::chat::threads::media::null::iterator::operator*(
) {
	return _that;
};

::ai::chat::threads::media::null::iterator
ai::chat::threads::media::null::insert_back(
	::ai::chat::timepoint_t timestamp,
	::ai::chat::string_t content,
	::ai::chat::tags_t tags
) {
	return iterator{
		timestamp,
		content,
		tags
	};
};

::ai::chat::threads::media::null::iterator
ai::chat::threads::media::null::begin(
) {
	return iterator{
		TheEndTimes,
		string_t{},
		tags_t{}
	};
};
::ai::chat::threads::media::null::iterator
ai::chat::threads::media::null::end() {
	return iterator{
		TheEndTimes,
		string_t{},
		tags_t{}
	};
};

#endif
