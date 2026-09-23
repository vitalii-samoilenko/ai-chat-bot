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
	::std::string_view partition,
	::std::string_view name,
	TThreadCluster &...threadCluster,
) : _partition{ partition }
	, _name{ name }
	, _threadCluster{ threadCluster ... }
	, _inputThread{ ::std::nullopt }
	, _commandThread{ ::std::nullopt } {

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
			::std::string_view{ "producer.partition" },
			::std::string_view{ _partition }
		),
		::std::make_tuple(
			::std::string_view{ "producer.name" },
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
		static ::RE2 a_receiver{ "@(\\w+)", ::RE2::Quiet };
		for (
			::std::string_view cursor{ content }, receiver{};
			::RE2::Consume(&cursor, a_receiver, &receiver);
		) {
			tags.push_back(
				::std::make_tuple(
					::std::string_view{ "receiver.name" },
					receiver
				)
			);
		}
	}
	if (!thread)
		return;
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
	partition_t partition{ input };
	::std::tuple<
		::std::string_view,
		::std::string_view
	> producer{};
	for (auto tag : tags) {
		if (::std::get<0>(tag) == "channel.partition") {
			if (::std::get<1>(tag) == "reject") {
				partition = reject;
			} else if (::std::get<1>(tag) == "review") {
				partition = review;
			} else if (::std::get<1>(tag) == "command") {
				partition = command;
			} else if (::std::get<1>(tag) == "unauthorized") {
				partition = unauthorized;
			} else if (::std::get<1>(tag) == "error") {
				partition = error;
			}
		} else if (::std::get<0>(tag) == "producer.partition") {
			::std::get<0>(producer) = ::std::get<1>(tag);
		} else if (::std::get<0>(tag) == "producer.name") {
			::std::get<1>(producer) = ::std::get<1>(tag);
		}
	}
	switch (partition) {
	case input:
		if (::std::get<0>(producer) == _partition
			&& ::std::get<1>(producer) == _name)
			break;
		::std::chrono::system_clock::time_point when{ timestamp };
		::std::cout << when
			<< " " << ::std::get<1>(producer)
			<< ": " << content
			<< ::std::endl;
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
