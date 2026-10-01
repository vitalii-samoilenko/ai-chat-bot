#include "gtest/gtest.h"

#include "ai/chat/threads/backed_tests.hpp"
#include "ai/chat/threads/media/null_tests.hpp"
//#include "ai/chat/repositories/pipe_tests.hpp"

int main(int argc, char **argv) {
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
};
