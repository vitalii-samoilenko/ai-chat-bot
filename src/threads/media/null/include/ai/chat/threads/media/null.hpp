#ifndef AI_CHAT_THREADS_MEDIA_NULL_HPP
#define AI_CHAT_THREADS_MEDIA_NULL_HPP

#include <span>
#include <string_view>
#include <tuple>

namespace ai {
namespace chat {
namespace threads {
namespace media {

class null {
private:
	static constexpr long long TheEndTimes;

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
		value_type that;

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
	iterator lower_bound(
		long long timestamp
	);
	iterator end();
};

} // media
} // threads
} // chat
} // ai

#include "ai/chat/threads/media/impl/null.ipp"

#endif
