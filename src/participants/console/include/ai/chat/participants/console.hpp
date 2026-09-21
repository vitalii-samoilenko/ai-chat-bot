#ifndef AI_CHAT_PARTICIPANTS_CONSOLE_HPP
#define AI_CHAT_PARTICIPANTS_CONSOLE_HPP

#include <optional>
#include <span>
#include <string>
#include <string_view>
#include <tuple>

namespace ai {
namespace chat {
namespace participants {

template<
	typename ...TThreadCluster
> class console {
private:
	::std::string _partition;
	::std::string _name;
	::std::tuple<
		TThreadCluster &...
	> _threadCluster;
	::std::optional<
		::std::tuple<
			::std::string,
			::std::string
		>
	> _inputThread;
	::std::optional<
		::std::tuple<
			::std::string,
			::std::string
		>
	> _commandThread;

public:
	console(
		::std::string_view partition,
		::std::string_view name,
		TThreadCluster &...threadCluster
	);
	console() = delete;
	console(console const &) = delete;
	console(console &&) = default;

	~console() = default;

	console &operator=(console const &) = delete;
	console &operator=(console &&) = default;

	void operator()();

	void join(
		::std::string_view partition,
		::std::string_view name
	);
	void leave(
		::std::string_view partition,
		::std::string_view name
	);

	void notify(
		long long timestamp,
		::std::string_view content,
		::std::span<
			::std::tuple<
				::std::string_view,
				::std::string_view
			>
		> tags
	);
};

} // participants
} // chat
} // ai

#include "ai/chat/participants/impl/console.ipp"

#endif
