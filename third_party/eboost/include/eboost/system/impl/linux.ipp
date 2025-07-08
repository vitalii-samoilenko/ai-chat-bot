#ifndef EBOOST_SYSTEM_LINUX_IPP
#define EBOOST_SYSTEM_LINUX_IPP

#include "eboost/system/memory.hpp"

#ifdef __unix__

#include <fstream>
#include <string>

#include <unistd.h>

namespace eboost {
namespace system {
namespace memory {

usage get_usage() {
    ::std::string path{ "/proc/" };
    path.append(::std::to_string(::getpid()));
    path.append("/statm");
    ::std::ifstream statm{ path };
    size_t page{ static_cast<size_t>(::getpagesize()) };
    usage usage{};
    if (statm.is_open()) {
        size_t _{};
        statm >> _;
        statm >> usage.total;
        statm >> usage.shared;
        usage.total *= page;
        usage.shared *= page;
    }
    return usage;
};

} // memory
} // system
} // eboost

#endif

#endif
