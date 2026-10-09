#ifndef AI_CHAT_REPOSITORIES_CONSOLE_HPP
#define AI_CHAT_REPOSITORIES_CONSOLE_HPP

#include <optional>
#include <string>

#include "ai/chat/model.hpp"
#include "ai/chat/participants/console.hpp"

namespace ai {
namespace chat {
namespace repositories {

template<
	typename TGlobalConfig,
	typename TRuntime,
	typename TThreadCluster
> class console {
public:
	class iterator {
	private:
		participants::console<
			TRuntime,
			TThreadCluster
		> *that;

		explicit iterator(
			participants::console<
				TRuntime,
				TThreadCluster
			> *that
		);

		friend console;

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
		participants::console<
			TRuntime,
			TThreadCluster
		>
		&operator*(
		);
		participants::console<
			TRuntime,
			TThreadCluster
		>
		*operator->(
		);
	};

private:
	::std::string const _partition;
	::std::optional<
		participants::console<
			TRuntime,
			TThreadCluster
		>
	> mutable _receiver;
	TGlobalConfig const &_globalConfig;
	TRuntime const &_runtime;
	TThreadCluster const &_threadCluster;

public:
	console(
		string_t partition,
		TGlobalConfig const &globalConfig,
		TRuntime const &_runtime,
		TThreadCluster const &threadCluster
	);
	console(
	) = delete;
	console(
		console const &
	) = delete;
	console(
		console &&
	) = default;

	~console(
	) = default;

	console
	&operator=(
		console const &
	) = delete;
	console
	&operator=(
		console &&
	) = delete;

	iterator
	begin(
	) const;
	iterator
	find(
		string_t partition,
		string_t name
	) const;
	iterator
	end(
	) const;
};

} // repositories
} // chat
} // ai

#include "ai/chat/repositories/impl/console.ipp"

#endif
