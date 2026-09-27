#ifndef AI_CHAT_THREADS_IMPL_BACKED_IPP
#define AI_CHAT_THREADS_IMPL_BACKED_IPP

#include <utility>
#include <vector>

#include "ai/chat/threads/backed.hpp"

template<
	typename TParticipantCluster,
	typename TMedia
> template<
	typename ...TMediaArgs
> ::ai::chat::threads::backed<
	TParticipantCluster,
	TMedia
>::backed(
	::std::string_view partition,
	::std::string_view name,
	TParticipantCluster const &participantCluster,
	TMediaArgs &&...mediaArgs
) : _partition{ partition }
	, _name{ name }
	, _participantCluster{ participantCluster } 
	, _participants{}
	, _messages{
		::std::forward<
			TMediaArgs
		>(mediaArgs) ...
	} {

};

template<
	typename TParticipantCluster,
	typename TMedia
> void
::ai::chat::threads::backed<
	TParticipantCluster,
	TMedia
>::accept(
	::std::string_view partition,
	::std::string_view name
) {
	_participants.insert(
		::std::make_tuple(
			::std::string{ partition },
			::std::string{ name }
		)
	);
};

template<
	typename TParticipantCluster,
	typename TMedia
> void
::ai::chat::threads::backed<
	TParticipantCluster,
	TMedia
>::dismiss(
	::std::string_view partition,
	::std::string_view name
) {
	_participants.erase(
		::std::make_tuple(
			::std::string{ partition },
			::std::string{ name }
		)
	);
};

template<
	typename TParticipantCluster,
	typename TMedia
> void
::ai::chat::threads::backed<
	TParticipantCluster,
	TMedia
>::push(
	::std::string_view content,
	::std::span<
		::std::tuple<
			::std::string_view,
			::std::string_view
		>
	> tags
) {
	::std::vector overrides{
		::std::make_tuple(
			::std::string_view{ "channel.partition" },
			::std::string_view{ _partition }
		),
		::std::make_tuple(
			::std::string_view{ "channel.name" },
			::std::string_view{ _name }
		)
	};
	for (auto tag : tags) {
		if (::std::get<0>(tag) == "channel.partition"
			|| ::std::get<0>(tag) == "channel.name")
			continue;
		overrides.push_back(tag);
	}
	auto message = _messages.insert_back(
		content,
		overrides
	);
	for (auto &participant : _participants) {
		try {
			_participantCluster.notify(
				::std::get<0>(participant),
				::std::get<1>(participant),
				::std::get<0>(*message),
				::std::get<1>(*message),
				::std::get<2>(*message)
			);
		} catch (...) {

		}
	}
};

#endif
