#ifndef AI_THREADS_IMPL_BACKED_IPP
#define AI_THREADS_IMPL_BACKED_IPP

#include <utility>

#include "ai/threads/backed.hpp"

namespace ai {
namespace threads {

template<
	typename TMedia,
	typename ...TParticipantGroup
> template<
	typename ...TMediaArgs
> backed<
	TMedia,
	TParticipantGroup ...
>::backed(
	TParticipantGroup &...participantGroup,
	TMediaArgs &&...mediaArgs
) : _participantGroup{ participantGroup... } 
	, _participants{}
	, _messages{
		::std::forward<
			TMediaArgs
		>(mediaArgs)...
	} {

};

template<
	typename TMedia,
	typename ...TParticipantGroup
> void backed<
	TMedia,
	TParticipantGroup ...
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
	typename ...TParticipantGroup
> void backed<
	TMedia,
	TParticipantGroup ...
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
	typename ...TParticipantGroup
> void backed<
	TMedia,
	TParticipantGroup ...
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
		::std::apply([&](TParticipantGroup &...participantGroup)->void {
			([&]()->bool {
				auto *instance = participantGroup.find(
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
		}, _participantGroup);
	}
};

} // threads
} // ai

#endif
