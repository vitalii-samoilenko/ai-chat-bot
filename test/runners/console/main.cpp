#include "gtest/gtest.h"

#include "ai/chat/threads/backed_tests.hpp"
#include "ai/chat/threads/media/null_tests.hpp"

using namespace ::testing;

int
main(
	int argc,
	char **argv
) {
	InitGoogleTest(
		&argc,
		argv
	);
	return RUN_ALL_TESTS();
};
