#ifndef EBOOST_SYSTEM_MEMORY_HPP
#define EBOOST_SYSTEM_MEMORY_HPP

namespace eboost {
namespace system {
namespace memory {

struct usage {
    size_t total;
    size_t shared;
};

usage get_usage();

} // memory
} // system
} // eboost

#include "impl/linux.ipp"
#include "impl/windows.ipp"

#endif
