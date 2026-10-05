#ifndef AI_CHAT_STUBS_CONSOLE_HPP
#define AI_CHAT_STUBS_CONSOLE_HPP

#include <string_view>

namespace ai {
namespace chat {
namespace stubs {

class console {
public:
	console(
	);
	console(
		console const &
	) = delete;
	console(
		console &&
	) = delete;

	~console(
	);

	console
	&operator=(
		console const &
	) = delete;
	console
	&operator=(
		console &&
	) = delete;

	void
	write(
		::std::string_view input
	) const;
	::std::string_view
	read(
	) const;
};

} // stubs
} // chat
} // ai

#include "ai/chat/stubs/impl/console.ipp"

#endif
