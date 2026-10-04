#ifndef AI_CHAT_THREADS_BACKED_HPP
#define AI_CHAT_THREADS_BACKED_HPP

#include <string>
#include <unordered_set>
#include <utility>

#include "ai/chat/model.hpp"

namespace ai {
namespace chat {
namespace threads {

template<
	typename TParticipantCluster,
	typename TMedia
> class backed {
public:
	using iterator = typename TMedia::iterator;

private:
	class hash {
	private:


	public:
		using is_transparent = void;

		template<
			typename T
		> size_t
		operator()(
			::std::pair<
				T,
				T
			> const &value
		) const {
			::std::hash<
				T
			> base{};
			return base(
				::std::get<0>(value)
			) ^ base(
				::std::get<1>(value)
			);
		};
	};
	class equal {
	private:
	

	public:
		using is_transparent = void;

		template<
			typename L,
			typename R
		> bool
		operator()(
			::std::pair<
				L,
				L
			> const &lhs,
			::std::pair<
				R,
				R
			> const &rhs
		) const {
			return ::std::get<0>(lhs) == ::std::get<0>(rhs)
				&& ::std::get<1>(lhs) == ::std::get<1>(rhs);
		};
	};

	::std::string const _partition;
	::std::string const _slot;
	TParticipantCluster const &_participantCluster;
	::std::unordered_set<
		::std::pair<
			::std::string,
			::std::string
		>,
		hash,
		equal
	> _participants;
	TMedia _messages;

public:
	template<
		typename ...TMediaArgs
	> backed(
		string_t partition,
		string_t name,
		TParticipantCluster const &participantCluster,
		TMediaArgs &&...mediaArgs
	);
	backed(
	) = delete;
	backed(
		backed const &
	) = delete;
	backed(
		backed &&
	) = default;

	~backed(
	) = default;

	backed
	&operator=(
		backed const &
	) = delete;
	backed
	&operator=(
		backed &&
	) = default;

	void
	accept(
		string_t partition,
		string_t slot
	);
	void
	dismiss(
		string_t partition,
		string_t slot
	);

	void
	push(
		string_t content,
		tags_t tags
	);
};

#include "ai/chat/threads/impl/backed.ipp"

} // threads
} // chat
} // ai

#endif
