#ifndef AI_CHAT_REPOSITORIES_IMPL_CONSOLE_IPP
#define AI_CHAT_REPOSITORIES_IMPL_CONSOLE_IPP

#include "ai/chat/repositories/console.hpp"

template<
	typename TGlobalConfig,
	typename TRuntime,
	typename TThreadCluster
> ::ai::chat::repositories::console<
	TGlobalConfig,
	TRuntime,
	TThreadCluster
>::iterator::iterator(
	::ai::chat::participants::console<
		TRuntime,
		TThreadCluster
	> *that
) : that{ that } {

};

template<
	typename TGlobalConfig,
	typename TRuntime,
	typename TThreadCluster
> bool
::ai::chat::repositories::console<
	TGlobalConfig,
	TRuntime,
	TThreadCluster
>::iterator::operator==(
	::ai::chat::repositories::console<
		TGlobalConfig,
		TRuntime,
		TThreadCluster
	>::iterator const &other
) const {
	return that == other.that;
};
template<
	typename TGlobalConfig,
	typename TRuntime,
	typename TThreadCluster
> ::ai::chat::repositories::console<
	TGlobalConfig,
	TRuntime,
	TThreadCluster
>::iterator
&::ai::chat::repositories::console<
	TGlobalConfig,
	TRuntime,
	TThreadCluster
>::iterator::operator++(
) {
	that = nullptr;
	return *this;
};
template<
	typename TGlobalConfig,
	typename TRuntime,
	typename TThreadCluster
> ::ai::chat::participants::console<
	TRuntime,
	TThreadCluster
>
&::ai::chat::repositories::console<
	TGlobalConfig,
	TRuntime,
	TThreadCluster
>::iterator::operator*(
) {
	return *that;
};
template<
	typename TGlobalConfig,
	typename TRuntime,
	typename TThreadCluster
> ::ai::chat::participants::console<
	TRuntime,
	TThreadCluster
>
*::ai::chat::repositories::console<
	TGlobalConfig,
	TRuntime,
	TThreadCluster
>::iterator::operator->(
) {
	return that;
};

template<
	typename TGlobalConfig,
	typename TRuntime,
	typename TThreadCluster
> ::ai::chat::repositories::console<
	TGlobalConfig,
	TRuntime,
	TThreadCluster
>::console(
	::ai::chat::string_t partition,
	TGlobalConfig const &globalConfig,
	TRuntime const &runtime,
	TThreadCluster const &threadCluster
) : _partition{ partition }
	, _receiver{ ::std::nullopt }
	, _globalConfig{ globalConfig }
	, _runtime{ runtime }
	, _threadCluster{ threadCluster } {

};

template<
	typename TGlobalConfig,
	typename TRuntime,
	typename TThreadCluster
> ::ai::chat::repositories::console<
	TGlobalConfig,
	TRuntime,
	TThreadCluster
>::iterator
ai::chat::repositories::console<
	TGlobalConfig,
	TRuntime,
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
	typename TRuntime,
	typename TThreadCluster
> ::ai::chat::repositories::console<
	TGlobalConfig,
	TRuntime,
	TThreadCluster
>::iterator
ai::chat::repositories::console<
	TGlobalConfig,
	TRuntime,
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
			_runtime,
			_threadCluster
		);
	}
	return iterator{
		&(*_receiver)
	};
};
template<
	typename TGlobalConfig,
	typename TRuntime,
	typename TThreadCluster
> ::ai::chat::repositories::console<
	TGlobalConfig,
	TRuntime,
	TThreadCluster
>::iterator
ai::chat::repositories::console<
	TGlobalConfig,
	TRuntime,
	TThreadCluster
>::end(
) const {
	return iterator{
		nullptr
	};
};

#endif
