#ifndef AI_CHAT_REPOSITORIES_PIPE_HPP
#define AI_CHAT_REPOSITORIES_PIPE_HPP

#include <string>
#include <string_view>
#include <tuple>
#include <unordered_map>

#include "ai/chat/threads/pipe.hpp"

namespace ai {
namespace chat {
namespace repositories {

template<
	typename TGlobalConfig,
	typename ...TParticipantCluster
> class pipe {
private:
	::std::string _partition;
	::std::unordered_map<
		::std::string,
		value_type
	> _channels;
	TGlobalConfig &_globalConfig;
	::std::tuple<
		TParticipantCluster &...
	> _participantCluster;

public:
	using value_type = threads::pipe;
	class iterator {
	private:
		::std::unordered_map<
			::std::string,
			value_type
		>::iterator that;

		explicit iterator(
			::std::unordered_map<
				::std::string,
				value_type
			>::iterator that
		);

		friend pipe;
	public:
		iterator() = delete;
		iterator(iterator const &) = delete;
		iterator(iterator &&) = default;

		~iterator() = default;

		iterator &operator=(iterator const &) = delete;
		iterator &operator=(iterator &&) = default;

		bool operator==(iterator const &other);
		iterator &operator++();
		value_type &operator*();
	};

	pipe(
		::std::string_view partition,
		TGlobalConfig &globalConfig,
		TParticipantCluster &...participantCluster
	);
	pipe() = delete;
	pipe(pipe const &) = delete;
	pipe(pipe &&) = delete;

	~pipe() = default;

	pipe &operator=(pipe const &) = delete;
	pipe &operator=(pipe &&) = delete;

	iterator begin();
	iterator find(
		::std::string_view partition,
		::std::string_view name
	);
	iterator end();
};

} // repositories
} // chat
} // ai

#include "ai/chat/repositories/impl/pipe.ipp"

#endif
