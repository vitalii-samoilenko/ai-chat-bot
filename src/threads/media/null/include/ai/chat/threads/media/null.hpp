#ifndef AI_CHAT_THREADS_MEDIA_NULL_HPP
#define AI_CHAT_THREADS_MEDIA_NULL_HPP

#include <limits>

#include "ai/chat/model.hpp"

namespace ai {
namespace chat {
namespace threads {
namespace media {

class null {
private:
	static constexpr long long TheEndTimes{
		::std::numeric_limits<
			timepoint_t
		>::max()
	};

public:
	class iterator {
	private:
		message_t that;

		iterator(
			timepoint_t timestamp,
			string_t content,
			tags_t tags
		);

		friend null;

	public:
		iterator(
		) = delete;
		iterator(
			iterator const &
		) = delete;
		iterator(
			iterator &&
		) = default;

		~iterator(
		) = default;

		iterator
		&operator=(
			iterator const &
		) = delete;
		iterator
		&operator=(
			iterator &&
		) = default;

		bool
		operator==(
			iterator const &other
		) const;
		bool
		operator<(
			iterator const &other
		) const;
		iterator
		&operator++(
		);
		message_t
		&operator*(
		);
	};

	null(
	) = default;
	null(
		null const &
	) = delete;
	null(
		null &&
	) = default;

	~null(
	) = default;

	null
	&operator=(
		null const &
	) = delete;
	null
	&operator=(
		null &&
	) = default;

	iterator
	insert_back(
		timepoint_t timestamp,
		string_t content,
		tags_t tags
	);

	iterator begin(
	);
	iterator end(
	);
};

} // media
} // threads
} // chat
} // ai

#include "ai/chat/threads/media/impl/null.ipp"

#endif
