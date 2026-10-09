#ifndef AI_CHAT_PARTICIPANTS_IMPL_CONSOLE_IPP
#define AI_CHAT_PARTICIPANTS_IMPL_CONSOLE_IPP

#include <chrono>
#include <sstream>
#include <string_view>
#include <vector>

#include "re2/re2.h"

#include "ai/chat/participants/console.hpp"

size_t g_console_count{ 0 };

void
console_open(
);
bool
console_try_read(
	::std::string_view *in
);
void
console_write(
	::std::string_view out
);
void
console_close(
);

template<
	typename TRuntime,
	typename TThreadCluster
> ::ai::chat::participants::console<
	TRuntime,
	TThreadCluster
>::console(
	::ai::chat::string_t partition,
	::ai::chat::string_t slot,
	TRuntime const &runtime,
	TThreadCluster const &threadCluster
) : _partition{ partition }
	, _slot{ slot }
	, _runtime{ runtime }
	, _threadCluster{ threadCluster }
	, _inputThread{ ::std::nullopt }
	, _commandThread{ ::std::nullopt } {
	if (++g_console_count == 1)
		::console_open();
	_runtime.schedule(
		_partition,
		_slot
	);
};
template<
	typename TRuntime,
	typename TThreadCluster
> ::ai::chat::participants::console<
	TRuntime,
	TThreadCluster
>::console(
	::ai::chat::participants::console<
		TRuntime,
		TThreadCluster
	> &&other
) : _partition{ ::std::move(other._partition) }
	, _slot{ ::std::move(other._slot) }
	, _runtime{ ::std::move(other._runtime) }
	, _threadCluster{ ::std::move(other._threadCluster) }
	, _inputThread{ ::std::move(other._inputThread) }
	, _commandThread{ ::std::move(other._commandThread) } {
	++g_console_count;
};

template<
	typename TRuntime,
	typename TThreadCluster
> ::ai::chat::participants::console<
	TRuntime,
	TThreadCluster
>::~console(
) {
	if (--g_console_count == 0)
		::console_close();
};

template<
	typename TRuntime,
	typename TThreadCluster
> void
::ai::chat::participants::console<
	TRuntime,
	TThreadCluster
>::operator()(
) const {
	_runtime.schedule(
		_partition,
		_slot
	);
	::std::string_view line{};
	if (!::console_try_read(&line)
		|| line.empty())
		return;
	string_t content{ line };
	::std::vector<
		tag_t
	> _tags{
		tag_t{ "producer.partition", _partition },
		tag_t{ "producer.slot", _slot }
	};
	::std::optional<
		::std::pair<
			::std::string_view,
			::std::string_view
		>
	> thread{ _inputThread };
	if (content[0] == '~') {
		content = content.substr(1);
		thread = _commandThread;
	} else {
		static const ::RE2 a_receiver{
			"@(\\w+)",
			::RE2::Quiet
		};
		for (::std::string_view cursor{ content }
				, receiver{}
				;::RE2::FindAndConsume(&cursor, a_receiver, &receiver)
				;)
			_tags.push_back(
				tag_t{ "receiver.slot", receiver }
			);
	}
	if (!thread)
		return;
	try {
		_threadCluster.push(
			::std::get<0>(*thread),
			::std::get<1>(*thread),
			content,
			_tags
		);
	} catch (...) {

	}
};

template<
	typename TRuntime,
	typename TThreadCluster
> void
::ai::chat::participants::console<
	TRuntime,
	TThreadCluster
>::join(
	::ai::chat::string_t partition,
	::ai::chat::string_t slot
) {
	::std::optional<
		::std::pair<
			::std::string,
			::std::string
		>
	> &thread{
		partition == "command"
			? _commandThread
			: _inputThread
	};
	thread = ::std::make_pair(
		::std::string{ partition },
		::std::string{ slot }
	);
};
template<
	typename TRuntime,
	typename TThreadCluster
> void
::ai::chat::participants::console<
	TRuntime,
	TThreadCluster
>::leave(
	::ai::chat::string_t partition,
	::ai::chat::string_t slot
) {
	::std::optional<
		::std::pair<
			::std::string,
			::std::string
		>
	> &thread{
		partition == "command"
			? _commandThread
			: _inputThread
	};
	if (!thread
		|| ::std::get<0>(*thread) != partition
		|| ::std::get<1>(*thread) != slot)
		return;
	thread = ::std::nullopt;
};

template<
	typename TRuntime,
	typename TThreadCluster
> void
::ai::chat::participants::console<
	TRuntime,
	TThreadCluster
>::notify(
	::ai::chat::timepoint_t timestamp,
	::ai::chat::string_t content,
	::ai::chat::tags_t tags
) const {
	enum type_t{
		input, reject, review,
		command, unauthorize, error
	};
	type_t type{ input };
	string_t partition{};
	string_t slot{};
	for (tag_t &tag : tags) {
		if (::std::get<0>(tag) == "channel.partition") {
			if (::std::get<1>(tag) == "reject") {
				type = reject;
			} else if (::std::get<1>(tag) == "review") {
				type = review;
			} else if (::std::get<1>(tag) == "command") {
				type = command;
			} else if (::std::get<1>(tag) == "unauthorize") {
				type = unauthorize;
			} else if (::std::get<1>(tag) == "error") {
				type = error;
			}
		} else if (::std::get<0>(tag) == "producer.partition") {
			partition = ::std::get<1>(tag);
		} else if (::std::get<0>(tag) == "producer.slot") {
			slot = ::std::get<1>(tag);
		}
	}
	::std::ostringstream sout{};
	sout << ::std::chrono::system_clock::time_point{
			::std::chrono::system_clock::duration{
				timestamp
			}
		};
	switch (type) {
	case input:
	case command:
			sout << " " << partition << "/" << slot
				<< ": " << content;
		break;
	case reject:
		sout << ": Message is rejected";
		break;
	case review:
		sout << ": Message is in review";
		break;
	case unauthorize:
		sout << ": Command not authorized";
		break;
	case error:
		sout << ": Command failed";
		break;
	}
	sout << ::std::endl;
	::std::string _line{ sout.str() };
	::console_write(_line);
};

#include "ai/chat/participants/impl/linux_console.ipp"

#endif
