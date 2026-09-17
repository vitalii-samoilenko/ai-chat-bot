#ifndef AI_THREADS_MEDIA_NULL_HPP
#define AI_THREADS_MEDIA_NULL_HPP

#include <span>
#include <string_view>
#include <tuple>

namespace ai {
namespace threads {
namespace media {

class null {
public:
	using value_type = ::std::tuple<
		long long,
		::std::string_view,
		::std::span<
			::std::tuple<
				::std::string_view
				::std::string_view
			>
		>
	>;
	class iterator {
	private:
		value_type _message;

		iterator(
			long long timestamp,
			::std::string_view _content,
			::std::span<
				::std::tuple<
					::std::string_view,
					::std::string_view
				>
			> tags
		);

		friend null;
	public:
		iterator() = delete;
		iterator(iterator const &) = delete;
		iterator(iterator &&) = default;

		~iterator() = default;

		iterator &operator=(iterator const &) = delete;
		iterator &operator=(iterator &&) = default;

		bool operator==(iterator const &other);
		iterator &operator++();
		value_type &operator*();
	};

	null() = default;
	null(null const &) = delete;
	null(null &&) = default;

	~null() = default;

	null &operator=(null const &) = delete;
	null &operator=(null &&) = default;

	iterator insert_back(
		::std::string_view content,
		::std::span<
			::std::tuple<
				::std::string_view,
				::std::string_view
			>
		> tags
	);

	iterator begin();
	iterator lower_bound(long long timestamp);
	iterator end();
};

} // media
} // threads
} // ai

#include "ai/threads/media/impl/null.ipp"

#endif
