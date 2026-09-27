#ifndef AI_CHAT_REPOSITORIES_CONSOLE_HPP
#define AI_CHAT_REPOSITORIES_CONSOLE_HPP

#include <optional>
#include <string>
#include <string_view>

#include "ai/chat/participants/console.hpp"

namespace ai {
namespace chat {
namespace repositories {

template<
	typename TGlobalConfig,
	typename TThreadCluster
> class console {
public:
	using value_type = participants::console<
		TThreadCluster
	>;
	class iterator {
	private:
		value_type *that;

		explicit iterator(
			value_type *that
		);

		friend console;

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
	::std::optional<
		value_type
	> mutable _recepient;
	TGlobalConfig const &_globalConfig;
	TThreadCluster const &_threadCluster;

public:
	console(
		::std::string_view partition,
		TGlobalConfig const &globalConfig,
		TThreadCluster const &threadCluster
	);
	console() = delete;
	console(console const &) = delete;
	console(console &&) = default;

	~console() = default;

	console &operator=(console const &) = delete;
	console &operator=(console &&) = delete;

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

#include "ai/chat/repositories/impl/console.ipp"

#endif
