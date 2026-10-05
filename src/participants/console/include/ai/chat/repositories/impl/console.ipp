#ifndef AI_CHAT_REPOSITORIES_IMPL_CONSOLE_IPP
#define AI_CHAT_REPOSITORIES_IMPL_CONSOLE_IPP

#include "ai/chat/repositories/console.hpp"

template<
	typename TGlobalConfig,
	typename TThreadCluster
> ::ai::chat::repositories::console<
	TGlobalConfig,
	TThreadCluster
>::iterator::iterator(
	::ai::chat::participants::console<
		TThreadCluster
	> *that
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
>::iterator::operator++(
) {
	that = nullptr;
	return *this;
};
template<
	typename TGlobalConfig,
	typename TThreadCluster
> ::ai::chat::participants::console<
	TThreadCluster
>
&::ai::chat::repositories::console<
	TGlobalConfig,
	TThreadCluster
>::iterator::operator*(
) {
	return *that;
};
template<
	typename TGlobalConfig,
	typename TThreadCluster
> ::ai::chat::participants::console<
	TThreadCluster
>
*::ai::chat::repositories::console<
	TGlobalConfig,
	TThreadCluster
>::iterator::operator->(
) {
	return that;
};

template<
	typename TGlobalConfig,
	typename TThreadCluster
> ::ai::chat::repositories::console<
	TGlobalConfig,
	TThreadCluster
>::console(
	::ai::chat::string_t partition,
	TGlobalConfig const &globalConfig,
	TThreadCluster const &threadCluster
) : _partition{ partition }
	, _receiver{ ::std::nullopt }
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
>::begin(
) const {
	if (!_receiver)
		return iterator{
			nullptr
		};
	return iterator{
		&(*_receiver)
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
	::ai::chat::string_t partition,
	::ai::chat::string_t name
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
	string_t allowed{
		_globalConfig.get_participant()
			.get_console()
			.get_slot()
	};
	if (!enabled || name != allowed)
		return iterator{
			nullptr
		};
	if (!_receiver) {
	/*
		_receiver = value_type{
			partition,
			name,
			_threadCluster
		};
	*/
		_receiver.emplace(
			partition,
			name,
			_threadCluster
		);
	}
	return iterator{
		&(*_receiver)
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
>::end(
) const {
	return iterator{
		nullptr
	};
};

#endif
