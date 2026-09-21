#ifndef AI_CHAT_REPOSITORIES_IMPL_CONSOLE_IPP
#define AI_CHAT_REPOSITORIES_IMPL_CONSOLE_IPP

template<
	typename ...TThreadCluster
> ::ai::chat::repositories::console<
	TThreadCluster ...
>::iterator::iterator(
	::ai::chat::repositories::console<
		TThreadCluster ...
	>::value_type *that
) : that{ that } {

};

template<
	typename ...TThreadCluster
> bool
::ai::chat::repositories::console<
	TThreadCluster ...
>::iterator::operator==(
	::ai::chat::repositories::console<
		TThreadCluster ...
	>::iterator const &other
) {
	return that == other.that;
};
template<
	typename ...TThreadCluster
> ::ai::chat::repositories::console<
	TThreadCluster ...
>::iterator
&::ai::chat::repositories::console<
	TThreadCluster ...
>::iterator::operator++() {
	that = nullptr;
	return *this;
};
template<
	typename ...TThreadCluster
> ::ai::chat::repositories::console<
	TThreadCluster ...
>::value_type
&::ai::chat::repositories::console<
	TThreadCluster ...
>::iterator::operator*() {
	return *that;
};

template<
	typename ...TThreadCluster
> ::ai::chat::repositories::console<
	TThreadCluster ...
>::console(
	::std::string_view partition,
	TThreadCluster &...threadCluster
) : _partition{ partition }
	, _name{}
	, _threadCluster{ threadCluster }
	, _recepient{ ::std::nullopt } {

};

template<
	typename ...TThreadCluster
> ::ai::chat::repositories::console<
	TThreadCluster ...
>::iterator
::ai::chat::repositories::console<
	TThreadCluster ...
>::begin() {
	return iterator{
		_recepient
			? &(*_recepient)
			: nullptr
	};
};
template<
	typename ...TThreadCluster
> ::ai::chat::repositories::console<
	TThreadCluster ...
>::iterator
::ai::chat::repositories::console<
	TThreadCluster ...
>::find(
	::std::string_view partition,
	::std::string_view name
) {
	if (partition != _partition)
		return iterator{
			nullptr
		};
	if (!_recepient) {
		_name = name;
		_recepient = ::std::apply([&](TThreadCluster &...threadCluster)->value_type {
			return value_type{
				partition,
				name,
				threadCluster ...
			};
		}, _threadCluster);
	}
	if (name != _name)
		return iterator{
			nullptr
		};
	return iterator{
		&(*_recepient)
	};
};
template<
	typename ...TThreadCluster
> ::ai::chat::repositories::console<
	TThreadCluster ...
>::iterator
::ai::chat::repositories::console<
	TThreadCluster ...
>::end() {
	return iterator{
		nullptr
	};
};

#endif
