#ifndef ESTD_ALGORITHM_HPP
#define ESTD_ALGORITHM_HPP

#include <type_traits>

#include <algorithm>

namespace estd {

struct equal {
	using is_transparent = ::std::true_type;

	template<
		typename T1,
		typename T2
	> bool
	operator()(
		T1 const &lhs,
		T2 const &rhs
	) const {
		return lhs == rhs;
	};
};

} // estd

#endif
