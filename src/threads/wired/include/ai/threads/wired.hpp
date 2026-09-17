#ifndef AI_THREADS_WIRED_HPP
#define AI_THREADS_WIRED_HPP

#include <span>
#include <string>
#include <string_view>
#include <tuple>
#include <uonrdered_set>

namespace ai {
namespace threads {

template<
	typename TChannel,
	typename ...TParticipantGroup
> class wired {
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
	TChannel _messages;

public:
	template<
		typename ...TChannelArgs
	> wired(
		TParticipantGroup &...participantGroup,
		TChannelArgs &&...channelArgs
	);
	wired() = delete;
	wired(wired const &) = delete;
	wired(wired &&) = default;

	~wired() = default;

	wired & operator=(wired const &) = delete;
	wired & operator=(wired &&) = default;

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

#include "impl/wired.ipp"

} // threads
} // ai

#endif
