#ifndef AI_CHAT_THREADS_PIPE_HPP
#define AI_CHAT_THREADS_PIPE_HPP

#include "ai/chat/threads/backed.hpp"
#include "ai/chat/threads/media/null.hpp"

namespace ai {
namespace chat {
namespace threads {

using pipe = backed<
	media::null
>;

} // threads
} // chat
} // ai

#endif
