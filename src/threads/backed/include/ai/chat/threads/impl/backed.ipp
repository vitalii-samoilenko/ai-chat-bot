#ifndef AI_CHAT_THREADS_IMPL_BACKED_IPP
#define AI_CHAT_THREADS_IMPL_BACKED_IPP

#include <chrono>
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
	::ai::chat::string_t partition,
	::ai::chat::string_t slot,
	TParticipantCluster const &participantCluster,
	TMediaArgs &&...mediaArgs
) : _partition{ partition }
	, _slot{ slot }
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
	::ai::chat::string_t partition,
	::ai::chat::string_t slot
) {
	_participants.insert(
		::std::make_pair(
			::std::string{ partition },
			::std::string{ slot }
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
	::ai::chat::string_t partition,
	::ai::chat::string_t slot
) {
	_participants.erase(
		::std::make_pair(
			partition,
			slot
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
	::ai::chat::string_t content,
	::ai::chat::tags_t tags
) {
	timepoint_t timestamp{
		::std::chrono::system_clock::now()
			.time_since_epoch()
			.count()
		};
	::std::vector<
		tag_t
	> _overrides{
		tag_t{ "channel.partition", _partition },
		tag_t{ "channel.slot", _slot }
	};
	for (tag_t &tag : tags) {
		if (get_name(tag) == "channel.partition"
			|| get_name(tag) == "channel.slot")
			continue;
		_overrides.push_back(tag);
	}
	tags_t overrides{ _overrides };
	auto message = _messages.insert_back(
		timestamp,
		content,
		overrides
	);
	for (auto &participant : _participants) {
		try {
			_participantCluster.notify(
				::std::get<0>(participant),
				::std::get<1>(participant),
				get_timestamp(*message),
				get_content(*message),
				get_tags(*message)
			);
		} catch (...) {

		}
	}
};

#endif
