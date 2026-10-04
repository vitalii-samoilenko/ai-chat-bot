#ifndef AI_CHAT_REPOSITORIES_IMPL_PIPE_IPP
#define AI_CHAT_REPOSITORIES_IMPL_PIPE_IPP

#include "ai/chat/repositories/pipe.hpp"

template<
	typename TGlobalConfig,
	typename TParticipantCluster
> ::ai::chat::repositories::pipe<
	TGlobalConfig,
	TParticipantCluster
>::iterator::iterator(
	::std::unordered_map<
		::std::string,
		::ai::chat::threads::pipe<
			TParticipantCluster
		>,
		::estd::hash,
		::estd::equal
	>::iterator &&that
) : that{ ::std::move(that) } {

};

template<
	typename TGlobalConfig,
	typename TParticipantCluster
> bool
::ai::chat::repositories::pipe<
	TGlobalConfig,
	TParticipantCluster
>::iterator::operator==(
	::ai::chat::repositories::pipe<
		TGlobalConfig,
		TParticipantCluster
	>::iterator const &other
) const {
	return that == other.that;
};
template<
	typename TGlobalConfig,
	typename TParticipantCluster
> ::ai::chat::repositories::pipe<
	TGlobalConfig,
	TParticipantCluster
>::iterator
&::ai::chat::repositories::pipe<
	TGlobalConfig,
	TParticipantCluster
>::iterator::operator++(
) {
	++that;
	return *this;
};
template<
	typename TGlobalConfig,
	typename TParticipantCluster
> ::ai::chat::threads::pipe<
	TParticipantCluster
>
&::ai::chat::repositories::pipe<
	TGlobalConfig,
	TParticipantCluster
>::iterator::operator*(
) {
	return ::std::get<1>(*that);
};
template<
	typename TGlobalConfig,
	typename TParticipantCluster
> ::ai::chat::threads::pipe<
	TParticipantCluster
>
*::ai::chat::repositories::pipe<
	TGlobalConfig,
	TParticipantCluster
>::iterator::operator->(
) {
	return &::std::get<1>(*that);
};

template<
	typename TGlobalConfig,
	typename TParticipantCluster
> ::ai::chat::repositories::pipe<
	TGlobalConfig,
	TParticipantCluster
>::pipe(
	string_t partition,
	TGlobalConfig const &globalConfig,
	TParticipantCluster const &participantCluster
) : _partition{ partition }
	, _channels{}
	, _globalConfig{ globalConfig }
	, _participantCluster{ participantCluster } {

};

template<
	typename TGlobalConfig,
	typename TParticipantCluster
> ::ai::chat::repositories::pipe<
	TGlobalConfig,
	TParticipantCluster
>::iterator
ai::chat::repositories::pipe<
	TGlobalConfig,
	TParticipantCluster
>::begin(
) const {
	return iterator{
		_channels.begin()
	};
};
template<
	typename TGlobalConfig,
	typename TParticipantCluster
> ::ai::chat::repositories::pipe<
	TGlobalConfig,
	TParticipantCluster
>::iterator
ai::chat::repositories::pipe<
	TGlobalConfig,
	TParticipantCluster
>::find(
	string_t partition,
	string_t slot
) const {
	if (_partition != partition)
		return iterator{
			_channels.end()
		};
	bool enabled{
		_globalConfig.get_thread()
			.get_pipe()
			.get_enabled()
	};
	if (!enabled)
		return iterator{
			_channels.end()
		};
	auto channel = _channels.find(
		slot
	);
	if (channel == _channels.end()) {
		::std::tie(
			channel,
			::std::ignore
		) = _channels.insert(
			::std::make_pair(
				::std::string{ slot },
				threads::pipe<
					TParticipantCluster
				>{
					partition,
					slot,
					_participantCluster
				}
			)
		);
	}
	return iterator{
		::std::move(channel)
	};
};
template<
	typename TGlobalConfig,
	typename TParticipantCluster
> ::ai::chat::repositories::pipe<
	TGlobalConfig,
	TParticipantCluster
>::iterator
ai::chat::repositories::pipe<
	TGlobalConfig,
	TParticipantCluster
>::end(
) const {
	return iterator{
		_channels.end()
	};
};

#endif
