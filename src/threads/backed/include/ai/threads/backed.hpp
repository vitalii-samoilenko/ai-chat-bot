#ifndef AI_THREADS_BACKED_HPP
#define AI_THREADS_BACKED_HPP

#include <span>
#include <string>
#include <string_view>
#include <tuple>
#include <uonrdered_set>

namespace ai {
namespace threads {

template<
	typename TMedia,
	typename ...TParticipantGroup
> class backed {
private:
	::std::tuple<
		TParticipantGroup &...
	> _participantGroup;
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
		TParticipantGroup &...participantGroup,
		TMediaArgs &&...mediaArgs
	);
	backed() = delete;
	backed(backed const &) = delete;
	backed(backed &&) = default;

	~backed() = default;

	backed &operator=(backed const &) = delete;
	backed &operator=(backed &&) = default;

	void accept(
		::std::string_view group,
		::std::string_view name
	);
	void dismiss(
		::std::string_view group,
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

#include "ai/threads/impl/backed.ipp"

} // threads
} // ai

#endif
