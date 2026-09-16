#ifndef AI_THREADS_IMPL_WIRED_IPP
#define AI_THREADS_IMPL_WIRED_IPP

#include <utility>

#include "ai/threads/wired.hpp"

namespace ai {
namespace threads {

template<
	typename TChannel,
	typename ...TParticipantRepos
> template<
	typename ...TChannelArgs
> wired<
	TChannel,
	TParticipantRepos ...
>::wired(
	TParticipantRepos &...participantRepos,
	TChannelArgs &&...channelArgs
) : _participantRepos{ participantRepos... } 
	, _participants{}
	, _messages{
		::std::forward<
			TChannelArgs
		>(channelArgs)...
	} {

};

template<
	typename TChannel,
	typename ...TParticipantRepos
> void wired<
	TChannel,
	TParticipantRepos ...
>::accept(
	::std::string_view group,
	::std::string_view name
) {
	_participants.insert(
		::std::make_tuple(
			::std::string{ group }
			::std::string{ name }
		)
	);
};

template<
	typename TChannel,
	typename ...TParticipantRepos
> void wired<
	TChannel,
	TParticipantRepos ...
>::dismiss(
	::std::string_view group,
	::std::string_view name
) {
	_participants.erase(
		::std::make_tuple(
			::std::string{ group },
			::std::string{ name }
		)
	);
};

template<
	typename TChannel,
	typename ...TParticipantRepos
> void wired<
	TChannel,
	TParticipantRepos ...
>::push(
	::std::string_view content,
	::std::span<
		::std::tuple<
			::std::string_view,
			::std::string_view
		>
	> tags
) {
	auto message = _messages.insert_back(content, tags);
	for (auto &participant : _participants) {
		::std::apply([&](TParticipantRepos &...participantRepos)->void {
			([&]()->bool {
				auto *instance = participantRepos.find(
					::std::get<0>(participant),
					::std::get<1>(participant)
				);
				if (!instance)
					return false;
				try {
					instance->notify(
						::std::get<0>(message),
						::std::get<1>(message),
						::std::get<2>(message)
					);
				}
				catch (...) {

				}
				return true;
			}() || ...);
		}, _participantRepos);
	}
};

} // threads
} // ai

#endif
