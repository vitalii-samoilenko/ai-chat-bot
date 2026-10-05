#ifndef AI_CHAT_PARTICIPANTS_CONSOLE_HPP
#define AI_CHAT_PARTICIPANTS_CONSOLE_HPP

#include <optional>
#include <string>
#include <utility>

#include "ai/chat/model.hpp"

namespace ai {
namespace chat {
namespace participants {

template<
	typename TThreadCluster
> class console {
private:
	::std::string const _partition;
	::std::string const _slot;
	TThreadCluster const &_threadCluster;
	::std::optional<
		::std::pair<
			::std::string,
			::std::string
		>
	> _inputThread;
	::std::optional<
		::std::pair<
			::std::string,
			::std::string
		>
	> _commandThread;

public:
	console(
		string_t partition,
		string_t slot,
		TThreadCluster const &threadCluster
	);
	console(
	) = delete;
	console(
		console const &
	) = delete;
	console(
		console &&other
	);

	~console(
	);

	console
	&operator=(
		console const &
	) = delete;
	console
	&operator=(
		console &&
	) = default;

	void
	operator()(
	) const;

	void
	join(
		string_t partition,
		string_t slot
	);
	void
	leave(
		string_t partition,
		string_t slot
	);

	void
	notify(
		timepoint_t timestamp,
		string_t content,
		tags_t tags
	) const;
};

} // participants
} // chat
} // ai

#include "ai/chat/participants/impl/console.ipp"

#endif
