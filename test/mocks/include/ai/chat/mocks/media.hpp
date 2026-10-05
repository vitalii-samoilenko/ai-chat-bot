#ifndef AI_CHAT_MOCKS_MEDIA_HPP
#define AI_CHAT_MOCKS_MEDIA_HPP

#include "gmock/gmock.h"

#include "ai/chat/model.hpp"

using namespace ::testing;

namespace ai {
namespace chat {
namespace mocks {

class media {
public:
	using iterator = message_t *;

private:


public:
	template<
		typename A
	> explicit media(
		A &&configure
	) {
		configure(this);
	};
	media(
	) = delete;
	media(
		media const &
	) = delete;
	media(
		media &&
	) = default;

	~media(
	) = default;

	media
	&operator=(
		media const &
	) = delete;
	media
	&operator=(
		media &&
	) = default;

	MOCK_METHOD(
		iterator,
		begin, (
		)
	);
	MOCK_METHOD(
		iterator,
		insert_back, (
			timepoint_t,
			string_t,
			tags_t
		)
	);
	MOCK_METHOD(
		iterator,
		end, (
		)
	);
};

} // mocks
} // chat
} // ai

#endif
