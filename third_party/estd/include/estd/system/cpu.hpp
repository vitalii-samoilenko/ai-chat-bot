#ifndef ESTD_SYSTEM_CPU_HPP
#define ESTD_SYSTEM_CPU_HPP

namespace estd {
namespace system {
namespace cpu {

struct usage {
	size_t system;
	size_t user;
};

usage get_usage();

} // cpu
} // system
} // estd

#include "impl/linux.ipp"
#include "impl/windows.ipp"

#endif
