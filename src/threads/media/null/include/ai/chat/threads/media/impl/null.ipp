#ifndef AI_CHAT_THREADS_MEDIA_IMPL_NULL_IPP
#define AI_CHAT_THREADS_MEDIA_IMPL_NULL_IPP

#include <chrono>
#include <limits>

::ai::chat::threads::media::null::TheEndTimes{
	::std::numeric_limits<
		long long
	>::max()
};

::ai::chat::threads::media::null::iterator::iterator(
	long long timestamp,
	::std::string_view _content,
	::std::span<
		::std::tuple<
			::std::string_view,
			::std::string_view
		>
	> tags
) : that{
		timestamp,
		content,
		tags
	} {

};

bool
::ai::chat::threads::media::null::iterator::operator==(
	::ai::chat::threads::media::null::iterator const &other
) {
	return ::std::get<0>(that) == ::std::get<0>(other.that);
};
::ai::chat::threads::media::null::iterator
&::ai::chat::threads::media::null::iterator::operator++() {
	::std::get<0>(that) = TheEndTimes;
	::std::get<1>(that) = ::std::string_view{};
	::std::get<2>(that) = ::std::string_view{};
	return *this;
};
:;ai::threads::media::null::value_type
&::ai::chat::threads::media::null::iterator::operator*() {
	return that;
};

::ai::chat::threads::media::null::iterator
::ai::chat::threads::media::null::insert_back(
	::std::string_view content,
	::std::span<
		::std::tuple<
			::std::string_view,
			::std::string_view
		>
	> tags
) {
	return ::ai::chat::threads::media::null::iterator{
		::std::chrono::system_clock::now()
			.time_since_epoch()
			.count(),
		content,
		tags
	};
};

::ai::chat::threads::media::null::iterator
::ai::chat::threads::media::null::begin() {
	return ::ai::chat::threads::media::null::iterator{
		TheEndTimes,
		::std::string_view{},
		::std::span{}
	};
};
::ai::chat::threads::media::null::iterator
::ai::chat::threads::media::null::lower_bound(
	long long timestamp
) {
	return ::ai::chat::threads::media::null::iterator{
		TheEndTimes,
		::std::string_view{},
		::std::span{}
	};
};
::ai::chat::threads::media::null::iterator
::ai::chat::threads::media::null::end() {
	return ::ai::chat::threads::media::null::iterator{
		TheEndTimes,
		::std::string_view{},
		::std::span{}
	};
};

#endif
