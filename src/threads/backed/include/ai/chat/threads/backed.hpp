#ifndef AI_CHAT_THREADS_BACKED_HPP
#define AI_CHAT_THREADS_BACKED_HPP

#include <span>
#include <string>
#include <string_view>
#include <tuple>
#include <unordered_set>

namespace std {
	template<>
	class hash<
		tuple<
			string,
			string
		>
	> {
	private:


	public:
		hash() = default;
		hash(hash const &) = default;
		hash(hash &&) = default;

		~hash() = default;

		hash &operator=(hash const &) = default;
		hash &operator=(hash &&) = default;

		inline size_t operator()(
			tuple<
				string,
				string
			> const &value
		) const {
			hash<
				string
			> single{};
			return single(
				get<0>(value)
			) ^ single(
				get<1>(value)
			);
		};
	};
}

namespace ai {
namespace chat {
namespace threads {

template<
	typename TParticipantCluster,
	typename TMedia
> class backed {
public:
	using value_type = typename TMedia::value_type;
	using iterator = typename TMedia::iterator;

private:
	::std::string const _partition;
	::std::string const _name;
	TParticipantCluster const &_participantCluster;
	::std::unordered_set<
		::std::tuple<
			::std::string,
			::std::string
		>
	> _participants;
	TMedia _messages;

public:
	template<
		typename ...TMediaArgs
	> backed(
		::std::string_view partition,
		::std::string_view name,
		TParticipantCluster const &participantCluster,
		TMediaArgs &&...mediaArgs
	);
	backed() = delete;
	backed(backed const &) = delete;
	backed(backed &&) = default;

	~backed() = default;

	backed &operator=(backed const &) = delete;
	backed &operator=(backed &&) = default;

	void accept(
		::std::string_view partition,
		::std::string_view name
	);
	void dismiss(
		::std::string_view partition,
		::std::string_view name
	);

	void push(
		::std::string_view content,
		::std::span<
			::std::tuple<
				::std::string_view,
				::std::string_view
			>
		> tags
	);
};

#include "ai/chat/threads/impl/backed.ipp"

} // threads
} // chat
} // ai

#endif
