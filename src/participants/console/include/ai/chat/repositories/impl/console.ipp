#ifndef AI_CHAT_REPOSITORIES_IMPL_CONSOLE_IPP
#define AI_CHAT_REPOSITORIES_IMPL_CONSOLE_IPP

template<
	typename TGlobalConfig,
	typename TThreadCluster
> ::ai::chat::repositories::console<
	TGlobalConfig,
	TThreadCluster
>::iterator::iterator(
	::ai::chat::repositories::console<
		TGlobalConfig,
		TThreadCluster
	>::value_type *that
) : that{ that } {

};

template<
	typename TGlobalConfig,
	typename TThreadCluster
> bool
::ai::chat::repositories::console<
	TGlobalConfig,
	TThreadCluster
>::iterator::operator==(
	::ai::chat::repositories::console<
		TGlobalConfig,
		TThreadCluster
	>::iterator const &other
) const {
	return that == other.that;
};
template<
	typename TGlobalConfig,
	typename TThreadCluster
> ::ai::chat::repositories::console<
	TGlobalConfig,
	TThreadCluster
>::iterator
&::ai::chat::repositories::console<
	TGlobalConfig,
	TThreadCluster
>::iterator::operator++() {
	that = nullptr;
	return *this;
};
template<
	typename TGlobalConfig,
	typename TThreadCluster
> ::ai::chat::repositories::console<
	TGlobalConfig,
	TThreadCluster
>::value_type
&::ai::chat::repositories::console<
	TGlobalConfig,
	TThreadCluster
>::iterator::operator*() {
	return *that;
};
template<
	typename TGlobalConfig,
	typename TThreadCluster
> ::ai::chat::repositories::console<
	TGlobalConfig,
	TThreadCluster
>::value_type
*::ai::chat::repositories::console<
	TGlobalConfig,
	TThreadCluster
>::iterator::operator->() {
	return that;
};

template<
	typename TGlobalConfig,
	typename TThreadCluster
> ::ai::chat::repositories::console<
	TGlobalConfig,
	TThreadCluster
>::console(
	::std::string_view partition,
	TGlobalConfig const &globalConfig,
	TThreadCluster const &threadCluster
) : _partition{ partition }
	, _recepient{ ::std::nullopt }
	, _globalConfig{ globalConfig }
	, _threadCluster{ threadCluster } {

};

template<
	typename TGlobalConfig,
	typename TThreadCluster
> ::ai::chat::repositories::console<
	TGlobalConfig,
	TThreadCluster
>::iterator
ai::chat::repositories::console<
	TGlobalConfig,
	TThreadCluster
>::begin() const {
	if (!_recepient)
		return iterator{
			nullptr
		};
	return iterator{
		&(*_recepient)
	};
};
template<
	typename TGlobalConfig,
	typename TThreadCluster
> ::ai::chat::repositories::console<
	TGlobalConfig,
	TThreadCluster
>::iterator
ai::chat::repositories::console<
	TGlobalConfig,
	TThreadCluster
>::find(
	::std::string_view partition,
	::std::string_view name
) const {
	if (partition != _partition)
		return iterator{
			nullptr
		};
	bool enabled{
		_globalConfig.get_participant()
			.get_console()
			.get_enabled()
	};
	::std::string_view allowed{
		_globalConfig.get_participant()
			.get_console()
			.get_name()
	};
	if (!enabled || name != allowed)
		return iterator{
			nullptr
		};
	if (!_recepient) {
	/*
		_recepient = value_type{
			partition,
			name,
			_threadCluster
		};
	*/
		_recepient.emplace(
			partition,
			name,
			_threadCluster
		);
	}
	return iterator{
		&(*_recepient)
	};
};
template<
	typename TGlobalConfig,
	typename TThreadCluster
> ::ai::chat::repositories::console<
	TGlobalConfig,
	TThreadCluster
>::iterator
ai::chat::repositories::console<
	TGlobalConfig,
	TThreadCluster
>::end() const {
	return iterator{
		nullptr
	};
};

#endif
