#ifndef EBOOST_SYSTEM_LINUX_IPP
#define EBOOST_SYSTEM_LINUX_IPP

#ifdef __unix__

#include "eboost/system/memory.hpp"

#include <fstream>
#include <string>

#include <sys/times.h>
#include <unistd.h>

namespace eboost {
namespace system {

pid_t g_pid{ ::getpid() };
long g_ticksps{ ::sysconf(_SC_CLK_TCK) };
long g_pageb{ ::sysconf(_SC_PAGESIZE) };

namespace cpu {

usage get_usage() {
    static double nanopt{ 1000000000. / g_ticksps };
    usage usage{};
    ::tms times{};
    if (!(nanopt < 0) && !(::times(&times) == (time_t)-1)) {
        usage.system = static_cast<size_t>((times.tms_stime + times.tms_cstime) * nanopt);
        usage.user = static_cast<size_t>((times.tms_utime + times.tms_cutime) * nanopt);
    }
    return usage;
};

} // cpu
namespace memory {

usage get_usage() {
    static ::std::string path{ "/proc/" + ::std::to_string(g_pid) + "/statm" };
    usage usage{};
    ::std::ifstream statm{ path };
    if (statm.is_open()) {
        statm >> usage.anonymous;   // virtual
        statm >> usage.anonymous;   // resident
        statm >> usage.shared;      // shared
        usage.anonymous -= usage.shared;
        usage.anonymous *= g_pageb;
        usage.shared *= g_pageb;
    }
    return usage;
};

} // memory
} // system
} // eboost

#endif

#endif
