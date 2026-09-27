#ifndef AI_CHAT_REPOSITORIES_PIPE_HPP
#define AI_CHAT_REPOSITORIES_PIPE_HPP

#include <string>
#include <string_view>
#include <unordered_map>

#include "ai/chat/threads/pipe.hpp"

namespace ai {
namespace chat {
namespace repositories {

template<
	typename TGlobalConfig,
	typename TParticipantCluster
> class pipe {
public:
	using value_type = threads::pipe<
		TParticipantCluster
	>;
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

		bool operator==(iterator const &other) const;
		iterator &operator++();
		value_type &operator*();
		value_type *operator->();
	};

private:
	::std::string const _partition;
	::std::unordered_map<
		::std::string,
		value_type
	> mutable _channels;
	TGlobalConfig const &_globalConfig;
	TParticipantCluster const &_participantCluster;

public:
	pipe(
		::std::string_view partition,
		TGlobalConfig const &globalConfig,
		TParticipantCluster const &participantCluster
	);
	pipe() = delete;
	pipe(pipe const &) = delete;
	pipe(pipe &&) = default;

	~pipe() = default;

	pipe &operator=(pipe const &) = delete;
	pipe &operator=(pipe &&) = delete;

	iterator begin() const;
	iterator find(
		::std::string_view partition,
		::std::string_view name
	) const;
	iterator end() const;
};

} // repositories
} // chat
} // ai

#include "ai/chat/repositories/impl/pipe.ipp"

#endif
