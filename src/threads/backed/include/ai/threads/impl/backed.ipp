#ifndef AI_THREADS_IMPL_BACKED_IPP
#define AI_THREADS_IMPL_BACKED_IPP

#include <utility>

#include "ai/threads/backed.hpp"

template<
	typename TMedia,
	typename ...TParticipantCluster
> template<
	typename ...TMediaArgs
> ::ai::threads::backed<
	TMedia,
	TParticipantCluster ...
>::backed(
	TParticipantCluster &...participantCluster,
	TMediaArgs &&...mediaArgs
) : _participantCluster{ participantCluster... } 
	, _participants{}
	, _messages{
		::std::forward<
			TMediaArgs
		>(mediaArgs)...
	} {

};

template<
	typename TMedia,
	typename ...TParticipantCluster
> void ::ai::threads::backed<
	TMedia,
	TParticipantCluster ...
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
	typename TMedia,
	typename ...TParticipantCluster
> void ::ai::threads::backed<
	TMedia,
	TParticipantCluster ...
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
	typename TMedia,
	typename ...TParticipantCluster
> void ::ai::threads::backed<
	TMedia,
	TParticipantCluster ...
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
		::std::apply([&](TParticipantCluster &...participantCluster)->void {
			([&]()->bool {
				auto *instance = participantCluster.find(
					::std::get<0>(participant),
					::std::get<1>(participant)
				);
				if (!instance)
					return false;
				try {
					instance->notify(
						::std::get<0>(*message),
						::std::get<1>(*message),
						::std::get<2>(*message)
					);
				}
				catch (...) {

				}
				return true;
			}() || ...);
		}, _participantCluster);
	}
};

#endif
