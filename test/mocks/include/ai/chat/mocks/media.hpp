#ifndef AI_CHAT_MOCKS_MEDIA_HPP
#define AI_CHAT_MOCKS_MEDIA_HPP

#include "gmock/gmock.h"

#include "ai/chat/model.hpp"

using namespace ::testing;

namespace ai {
namespace chat {
namespace mocks {

class Media {
public:
	using iterator = message_t *;

private:


public:
	template<
		typename A
	> explicit Media(
		A &&configure
	) {
		configure(this);
	};
	Media(
	) = delete;
	Media(
		Media const &
	) = delete;
	Media(
		Media &&
	) = default;

	~Media(
	) = default;

	Media
	&operator=(
		Media const &
	) = delete;
	Media
	&operator=(
		Media &&
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
