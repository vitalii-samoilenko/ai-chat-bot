#ifndef EBOOST_SYSTEM_CPU_HPP
#define EBOOST_SYSTEM_CPU_HPP

namespace eboost {
namespace system {
namespace cpu {

struct usage {
    size_t system;
    size_t user;
};

usage get_usage();

} // cpu
} // system
} // eboost

#include "impl/linux.ipp"
#include "impl/windows.ipp"

#endif
