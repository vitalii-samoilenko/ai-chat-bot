#ifndef ESTD_SYSTEM_MEMORY_HPP
#define ESTD_SYSTEM_MEMORY_HPP

namespace estd {
namespace system {
namespace memory {

struct usage {
	size_t anonymous;
	size_t shared;
};

usage get_usage();

} // memory
} // system
} // estd

#include "impl/linux.ipp"
#include "impl/windows.ipp"

#endif
