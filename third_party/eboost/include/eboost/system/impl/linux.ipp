#ifndef EBOOST_SYSTEM_LINUX_IPP
#define EBOOST_SYSTEM_LINUX_IPP

#include "eboost/system/memory.hpp"

#ifdef __unix__

#include <fstream>
#include <string>

#include <sys/times.h>
#include <unistd.h>

namespace eboost {
namespace system {
namespace cpu {

usage get_usage() {
    usage usage{};
    long ticks{ ::sysconf( _SC_CLK_TCK ) };
    ::tms times{};
    if (!(ticks == -1) && !(::times(&times) == (time_t)-1)) {
        usage.system = static_cast<size_t>(times.tms_stime + times.tms_cstime) * ticks;
        usage.user = static_cast<size_t>(times.tms_utime + times.tms_cutime) * ticks;
    }
    return usage;
};

} // cpu
namespace memory {

usage get_usage() {
    ::std::string path{ "/proc/" };
    path.append(::std::to_string(::getpid()));
    path.append("/statm");
    ::std::ifstream statm{ path };
    size_t page{ static_cast<size_t>(::getpagesize()) };
    usage usage{};
    if (statm.is_open()) {
        statm >> usage.anonymous;   // virtual
        statm >> usage.anonymous;   // resident
        statm >> usage.shared;      // shared
        usage.anonymous -= usage.shared;
        usage.anonymous *= page;
        usage.shared *= page;
    }
    return usage;
};

} // memory
} // system
} // eboost

#endif

#endif
