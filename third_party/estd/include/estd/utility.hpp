#ifndef ESTD_UTILITY_HPP
#define ESTD_UTILITY_HPP

#include <type_traits>

#include <utility>

namespace estd {

struct hash {
	using is_transparent = ::std::true_type;

	template<
		typename T
	> size_t
	operator()(
		T const &key
	) const {
		return ::std::hash<T>{}(
			key
		);
	};
	template<
		typename T1,
		typename T2
	> size_t
	operator()(
		::std::pair<
			T1,
			T2
		> const &key
	) const {
		return ::std::hash<T1>{}(
			::std::get<0>(key)
		) ^ ::std::hash<T2>{}(
			::std::get<1>(key)
		);
	};
};

} // estd

#endif
