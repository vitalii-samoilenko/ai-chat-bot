#ifndef AI_CHAT_REPOSITORIES_IMPL_PIPE_IPP
#define AI_CHAT_REPOSITORIES_IMPL_PIPE_IPP

#include "ai/chat/repositories/pipe.hpp"

template
	typename TConfig,
	typename ...TParticipantCluster
> ::ai::chat::repositories::pipe<
	TConfig,
	TParticipantCluster ...
>::iterator::iterator(
	::std::unordered_map<
		::std::string,
		value_type
	>::iterator that
) : that{ that } {

};

template
	typename TConfig,
	typename ...TParticipantCluster
> bool
::ai::chat::repositories::pipe<
	TConfig,
	TParticipantCluster ...
>::iterator::operator==(
	::ai::chat::repositories::pipe<
		TConfig,
		TParticipantCluster ...
	>::iterator const &other
) {
	return that == other.that;
};
template
	typename TConfig,
	typename ...TParticipantCluster
> ::ai::chat::repositories::pipe<
	TConfig,
	TParticipantCluster ...
>::iterator
&::ai::chat::repositories::pipe<
	TConfig,
	TParticipantCluster ...
>::iterator::operator++() {
	++that;
	return *this;
};
template
	typename TConfig,
	typename ...TParticipantCluster
> ::ai::chat::repositories::pipe<
	TConfig,
	TParticipantCluster ...
>::value_type
&::ai::chat::repositories::pipe<
	TConfig,
	TParticipantCluster ...
>::iterator::operator*() {
	return ::std::get<1>(*that);
};

template
	typename TConfig,
	typename ...TParticipantCluster
> ::ai::chat::repositories::pipe<
	TConfig,
	TParticipantCluster ...
>::pipe(
	::std::string_view partition,
	TConfig &globalConfig,
	TParticipantCluster &...participantCluster
) : _partition{ partition }
	, _channels{}
	, _globalConfig{ globalConfig }
	, _participantCluster{ participantCluster ... } {

};

template
	typename TConfig,
	typename ...TParticipantCluster
> ::ai::chat::repositories::pipe<
	TConfig,
	TParticipantCluster ...
>::iterator
::ai::chat::repositories::pipe<
	TConfig,
	TParticipantCluster ...
>::begin() {
	return iterator{
		_channels.begin()
	};
};
template
	typename TConfig
> ::ai::chat::repositories::pipe<
	TConfig,
	TParticipantCluster ...
>::iterator
::ai::chat::repositories::pipe<
	TConfig,
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
		_globalConfig.get_threads()
			.get_pipes()
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
	typename TConfig,
	typename ...TParticipantCluster
> ::ai::chat::repositories::pipe<
	TConfig,
	TParticipantCluster ...
>::iterator
::ai::chat::repositories::pipe<
	TConfig,
	TParticipantCluster ...
>::end() {
	return iterator{
		_channels.end()
	};
};

#endif
