#ifndef AI_CHAT_PARTICIPANTS_IMPL_CONSOLE_IPP
#define AI_CHAT_PARTICIPANTS_IMPL_CONSOLE_IPP

#include <chrono>
#include <iostream>
#include <vector>

#include "re2/re2.h"

template<
	typename ...TThreadCluster
> ::ai::chat::participants::console<
	TThreadCluster ...
>::console(
	TThreadCluster &...threadCluster,
	::std::string_view name
) : _threadCluster{ threadCluster ... }
	, _inputThread{ ::std::nullopt }
	, _commandThread{ ::std::nullopt }
	, _name{ name } {

};

template<
	typename ...TThreadCluster
> void
::ai::chat::participants::console<
	TThreadCluster ...
>::operator()() {
	::std::string line{};
	::std::getline(::std::cin, line);
	if (line.empty())
		return;
	::std::string_view content{ line };
	::std::vector<
		::std::tuple<
			::std::string_view,
			::std::string_view
		>
	> tags{
		::std::make_tuple(
			::std::string_view{ "sender" },
			::std::string_view{ _name }
		)
	};
	::std::optional<
		::std::tuple<
			::std::string_view,
			::std::string_view
		>
	> thread{ _inputThread };
	if (content[0] == '~') {
		content = content.substr(1);
		thread = _commandThread;
	} else {
		static ::RE2 a_recepient{ "@(\\w+)", ::RE2::Quiet };
		for (
			::std::string_view cursor{ content }, recepient{};
			::RE2::Consume(&cursor, a_recepient, &recepient);
		) {
			tags.push_back(
				::std::make_tuple(
					::std::string_view{ "recepient" },
					recepient
				)
			);
		}
	}
	if (!thread)
		return;
	tags.push_back(
		::std::make_tuple(
			::std::string_view{ "partition" },
			::std::string_view{ ::std::get<0>(*thread) }
		)
	);
	::std::apply([&](TThreadCluster &...threadCluster)->void {
		([&]()->bool {
			auto channel = threadCluster.find(
				::std::get<0>(*thread),
				::std::get<1>(*thread)
			);
			if (channel == threadCluster.end())
				return false;
			channel->push(
				content,
				tags
			);
			return true;
		}() || ...);
	}, _threadCluster);
};

template<
	typename ...TThreadCluster
> void
::ai::chat::participants::console<
	TThreadCluster ...
>::join(
	::std::string_view partition,
	::std::string_view name
) {
	::std::optional<
		::std::tuple<
			::std::string,
			::std::string
		>
	> &thread{
		partition == "command"
			? _commandThread
			: _inputThread
	};
	thread = ::std::make_tuple(
		::std::string{ partition },
		::std::string{ name }
	);
};
template<
	typename ...TThreadCluster
> void
::ai::chat::participants::console<
	TThreadCluster ...
>::leave(
	::std::string_view partition,
	::std::string_view name
) {
	::std::optional<
		::std::tuple<
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
		|| ::std::get<1>(*thread) != name)
		return;
	thread = ::std::nullopt;
};

template<
	typename ...TThreadCluster
> void
::ai::chat::participants::console<
	TThreadCluster ...
>::notify(
	long long timestamp,
	::std::string_view content,
	::std::span<
		::std::tuple<
			::std::string_view,
			::std::string_view
		>
	> tags
) {
	enum partition_t{
		input, reject, review,
		command, unauthorized, error
	};
	enum sender_t{
		self, other
	};
	partition_t partition{ input };
	::std::string_view sender{};
	for (auto tag : tags) {
		if (::std::get<0>(tag) == "partition") {
			if (::std::get<1>(tag) == "command") {
				partition = command;
			} else if (::std::get<1>(tag) == "reject") {
				partition = reject;
			} else if (::std::get<1>(tag) == "review") {
				partition = review;
			} else if (::std::get<1>(tag) == "unauthorized") {
				partition = unauthorized;
			} else if (::std::get<1>(tag) == "error") {
				partition = error;
			}
		} else if (::std::get<0>(tag) == "sender") {
			sender = ::std::get<1>(tag);
		}
	}
	switch (partition) {
	case input:
		if (sender == _name)
			break;
		::std::chrono::system_clock::time_point when{ timestamp };
		::std::cout << when << " " << sender << ": " << content << ::std::endl;
		break;
	case reject:
		::std::cout << "Message is rejected" << ::std::endl;
		break;
	case review:
		::std::cout << "Message is in review" << ::std::endl;
		break;
	case unauthorized:
		::std::cout << "Command not authorized" << ::std::endl;
		break;
	case error:
		::std::cout << "Command failed" << ::std::endl;
		break;
	}
};

#endif
