#ifndef AI_THREADS_MEDIA_IMPL_NULL_IPP
#define AI_THREADS_MEDIA_IMPL_NULL_IPP

#include <chrono>
#include <limits>

::ai::threads::media::null::iterator::iterator(
	long long timestamp,
	::std::string_view _content,
	::std::span<
		::std::tuple<
			::std::string_view,
			::std::string_view
		>
	> tags
) : _message{
		timestamp,
		content,
		tags
	} {

};

bool ::ai::threads::media::null::iterator::operator==(
	::ai::threads::media::null::iterator const &other
) {
	return ::std::get<0>(_message) == ::std::get<0>(other._message);
};
::ai::threads::media::null::iterator
&::ai::threads::media::null::iterator::operator++() {
	::std::get<0>(_message) = ::std::numeric_limits<
		long long
	>::max();
	::std::get<1>(_message) = ::std::string_view{};
	::std::get<2>(_message) = ::std::string_view{};
};
:;ai::threads::media::null::value_type
&::ai::threads::media::null::iterator::operator*() {
	return _message;
};

::ai::threads::media::null::iterator
::ai::threads::media::null::insert_back(
	::std::string_view content,
	::std::span<
		::std::tuple<
			::std::string_view,
			::std::string_view
		>
	> tags
) {
	return ::ai::threads::media::null::iterator{
		::std::chrono::system_clock::now()
			.time_since_epoch()
			.count(),
		content,
		tags
	};
};

::ai::threads::media::null::iterator
::ai::threads::media::null::begin() {
	return ::ai::threads::media::null::iterator{
		::std::numeric_limits<
			long long
		>::max(),
		::std::string_view{},
		::std::span{}
	};
};
::ai::threads::media::null::iterator
::ai::threads::media::null::lower_bound(long long timestamp) {
	return ::ai::threads::media::null::iterator{
		::std::numeric_limits<
			long long
		>::max(),
		::std::string_view{},
		::std::span{}
	};
};
::ai::threads::media::null::iterator
::ai::threads::media::null::end() {
	return ::ai::threads::media::null::iterator{
		::std::numeric_limits<
			long long
		>::max(),
		::std::string_view{},
		::std::span{}
	};
};

#endif
