#ifndef AI_CHAT_MOCKS_RUNTIME_HPP
#define AI_CHAT_MOCKS_RUNTIME_HPP

#include "gmock/gmock.h"

using namespace ::testing;

namespace ai {
namespace chat {
namespace mocks {

class runtime {
private:


public:
	runtime(
	) = default;
	runtime(
		runtime const &
	) = delete;
	runtime(
		runtime &&
	) = delete;

	~runtime(
	) = default;

	runtime
	&operator=(
		runtime const &
	) = delete;
	runtime
	&operator=(
		runtime &&
	) = delete;

	MOCK_METHOD(
		void,
		schedule, (
			string_t,
			string_t
		),
		(const)
	);
};

} // mocks
} // chat
} // ai

#endif
