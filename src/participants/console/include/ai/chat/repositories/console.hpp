#ifndef AI_CHAT_REPOSITORIES_CONSOLE_HPP
#define AI_CHAT_REPOSITORIES_CONSOLE_HPP

#include <optional>
#include <string>
#include <string_view>
#include <tuple>
#include <unordered_map>

#include "ai/chat/participants/console.hpp"

namespace ai {
namespace chat {
namespace repositories {

template<
	typename TGlobalConfig,
	typename ...TThreadCluster
> class console {
private:
	::std::string _partition;
	::std::optional<
		value_type
	> _recepient;
	TGlobalConfig &_globalConfig;
	::std::tuple<
		TThreadCluster &...
	> _threadCluster;

public:
	using value_typle = participants::console<
		TThreadCluster ...
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

		bool operator==(iterator const &other);
		iterator &operator++();
		value_type &operator*();
	};

	console(
		::std::string_view partition,
		TGlobalConfig &globalConfig,
		TThreadCluster &...threadCluster
	);
	console() = delete;
	console(console const &) = delete;
	console(console &&) = delete;

	~console() = default;

	console &operator=(console const &) = delete;
	console &operator=(console &&) = delete;

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

#include "ai/chat/repositories/impl/console.ipp"

#endif
