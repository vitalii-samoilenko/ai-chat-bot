#ifndef AI_CHAT_THREADS_BACKED_HPP
#define AI_CHAT_THREADS_BACKED_HPP

#include <span>
#include <string>
#include <string_view>
#include <tuple>
#include <uonrdered_set>

namespace ai {
namespace chat {
namespace threads {

template<
	typename TMedia,
	typename ...TParticipantCluster
> class backed {
private:
	::std::tuple<
		TParticipantCluster &...
	> _participantCluster;
	::std::unordered_set<
		::std::tuple<
			::std::string,
			::std::string
		>
	> _participants;
	TMedia _messages;

public:
	template<
		typename ...TMediaArgs
	> backed(
		TParticipantCluster &...participantCluster,
		TMediaArgs &&...mediaArgs
	);
	backed() = delete;
	backed(backed const &) = delete;
	backed(backed &&) = default;

	~backed() = default;

	backed &operator=(backed const &) = delete;
	backed &operator=(backed &&) = default;

	void accept(
		::std::string_view partition,
		::std::string_view name
	);
	void dismiss(
		::std::string_view partition,
		::std::string_view name
	);

	void push(
		::std::string_view content,
		::std::span<
			::std::tuple<
				::std::string_view,
				::std::string_view
			>
		> tags
	);
};

#include "ai/chat/threads/impl/backed.ipp"

} // threads
} // chat
} // ai

#endif
