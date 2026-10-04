#ifndef AI_CHAT_REPOSITORIES_PIPE_HPP
#define AI_CHAT_REPOSITORIES_PIPE_HPP

#include <string>
#include <unordered_map>

#include "estd/algorithm.hpp"
#include "estd/utility.hpp"

#include "ai/chat/model.hpp"
#include "ai/chat/threads/pipe.hpp"

namespace ai {
namespace chat {
namespace repositories {

template<
	typename TGlobalConfig,
	typename TParticipantCluster
> class pipe {
public:
	class iterator {
	private:
		::std::unordered_map<
			::std::string,
			threads::pipe<
				TParticipantCluster
			>,
			::estd::hash,
			::estd::equal
		>::iterator that;

		explicit iterator(
			::std::unordered_map<
				::std::string,
				threads::pipe<
					TParticipantCluster
				>,
				::estd::hash,
				::estd::equal
			>::iterator &&that
		);

		friend pipe;

	public:
		iterator(
		) = delete;
		iterator(
			iterator const &
		) = delete;
		iterator(
			iterator &&
		) = default;

		~iterator(
		) = default;

		iterator
		&operator=(
			iterator const &
		) = delete;
		iterator
		&operator=(
			iterator &&
		) = default;

		bool
		operator==(
			iterator const &other
		) const;
		iterator
		&operator++(
		);
		threads::pipe<
			TParticipantCluster
		>
		&operator*(
		);
		threads::pipe<
			TParticipantCluster
		>
		*operator->(
		);
	};

private:
	::std::string const _partition;
	::std::unordered_map<
		::std::string,
		threads::pipe<
			TParticipantCluster
		>,
		::estd::hash,
		::estd::equal
	> mutable _channels;
	TGlobalConfig const &_globalConfig;
	TParticipantCluster const &_participantCluster;

public:
	pipe(
		string_t partition,
		TGlobalConfig const &globalConfig,
		TParticipantCluster const &participantCluster
	);
	pipe(
	) = delete;
	pipe(
		pipe const &
	) = delete;
	pipe(
		pipe &&
	) = default;

	~pipe(
	) = default;

	pipe
	&operator=(
		pipe const &
	) = delete;
	pipe
	&operator=(
		pipe &&
	) = delete;

	iterator
	begin(
	) const;
	iterator
	find(
		string_t partition,
		string_t slot
	) const;
	iterator
	end(
	) const;
};

} // repositories
} // chat
} // ai

#include "ai/chat/repositories/impl/pipe.ipp"

#endif
