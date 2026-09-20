#ifndef AI_CHAT_REPOSITORIES_IMPL_PIPE_IPP
#define AI_CHAT_REPOSITORIES_IMPL_PIPE_IPP

#include "ai/chat/repositories/pipe.hpp"

template
	typename TGlobalConfig,
	typename ...TParticipantCluster
> ::ai::chat::repositories::pipe<
	TGlobalConfig,
	TParticipantCluster ...
>::iterator::iterator(
	::std::unordered_map<
		::std::string,
		value_type
	>::iterator that
) : that{ that } {

};

template
	typename TGlobalConfig,
	typename ...TParticipantCluster
> bool
::ai::chat::repositories::pipe<
	TGlobalConfig,
	TParticipantCluster ...
>::iterator::operator==(
	::ai::chat::repositories::pipe<
		TGlobalConfig,
		TParticipantCluster ...
	>::iterator const &other
) {
	return that == other.that;
};
template
	typename TGlobalConfig,
	typename ...TParticipantCluster
> ::ai::chat::repositories::pipe<
	TGlobalConfig,
	TParticipantCluster ...
>::iterator
&::ai::chat::repositories::pipe<
	TGlobalConfig,
	TParticipantCluster ...
>::iterator::operator++() {
	++that;
	return *this;
};
template
	typename TGlobalConfig,
	typename ...TParticipantCluster
> ::ai::chat::repositories::pipe<
	TGlobalConfig,
	TParticipantCluster ...
>::value_type
&::ai::chat::repositories::pipe<
	TGlobalConfig,
	TParticipantCluster ...
>::iterator::operator*() {
	return ::std::get<1>(*that);
};

template
	typename TGlobalConfig,
	typename ...TParticipantCluster
> ::ai::chat::repositories::pipe<
	TGlobalConfig,
	TParticipantCluster ...
>::pipe(
	::std::string_view partition,
	TGlobalConfig &globalConfig,
	TParticipantCluster &...participantCluster
) : _partition{ partition }
	, _channels{}
	, _globalConfig{ globalConfig }
	, _participantCluster{ participantCluster ... } {

};

template
	typename TGlobalConfig,
	typename ...TParticipantCluster
> ::ai::chat::repositories::pipe<
	TGlobalConfig,
	TParticipantCluster ...
>::iterator
::ai::chat::repositories::pipe<
	TGlobalConfig,
	TParticipantCluster ...
>::begin() {
	return iterator{
		_channels.begin()
	};
};
template
	typename TGlobalConfig
> ::ai::chat::repositories::pipe<
	TGlobalConfig,
	TParticipantCluster ...
>::iterator
::ai::chat::repositories::pipe<
	TGlobalConfig,
	TParticipantCluster ...
>::find(
	::std::string_view partition,
	::std::string_view name
) {
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
	auto channel = _channels.find(name);
	if (channel == _channels.end() {
		::std::tie(channel, ::std::ignored) = _channels.insert(
			::std::make_pair(
				::std::string{ name },
				::std::apply([&](TParticipantCluster ...participantCluster)->threads::pipe {
					return threads::pipe{
						participantCluster ...
					};
				}, _participantCluster)
			)
		);
	}
	return iterator{
		channel
	};
};
template
	typename TGlobalConfig,
	typename ...TParticipantCluster
> ::ai::chat::repositories::pipe<
	TGlobalConfig,
	TParticipantCluster ...
>::iterator
::ai::chat::repositories::pipe<
	TGlobalConfig,
	TParticipantCluster ...
>::end() {
	return iterator{
		_channels.end()
	};
};

#endif
