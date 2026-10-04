#ifndef ESTD_SYSTEM_HPP
#define ESTD_SYSTEM_HPP

namespace estd {
namespace cpu {

struct usage {
	size_t system;
	size_t user;
};

usage
get_usage(
);

} // cpu
namespace memory {

struct usage {
	size_t anonymous;
	size_t shared;
};

usage
get_usage(
);

} // memory
} // estd

#include "estd/impl/linux.ipp"
#include "estd/impl/windows.ipp"

#endif
