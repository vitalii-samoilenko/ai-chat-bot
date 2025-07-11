#ifndef EBOOST_SYSTEM_WINWDOWS_IPP
#define EBOOST_SYSTEM_WINWDOWS_IPP

#ifdef _WIN32

#include "eboost/system/memory.hpp"

#include <windows.h>
#include <processthreadsapi.h>
#include <psapi.h>

namespace eboost {
namespace system {

::HANDLE g_hProcess{ ::GetCurrentProcess() };

namespace cpu {

usage get_usage() {
    usage usage{};
    ::FILETIME creation{};
    ::FILETIME exit{};
    ::FILETIME kernel{};
    ::FILETIME user{};
    if (::GetProcessTimes(g_hProcess, &creation, &exit, &kernel, &user)) {
        usage.system = ((static_cast<size_t>(kernel.dwHighDateTime) << 32) | kernel.dwLowDateTime) * 100;
        usage.user = ((static_cast<size_t>(user.dwHighDateTime) << 32) | user.dwLowDateTime) * 100;
    }
    return usage;
};

} // cpu
namespace memory {

usage get_usage() {
    usage usage{};
    ::PROCESS_MEMORY_COUNTERS_EX2 counters{};
    if (::GetProcessMemoryInfo(g_hProcess, reinterpret_cast<PROCESS_MEMORY_COUNTERS*>(&counters), sizeof(counters))) {
        usage.anonymous = counters.PrivateWorkingSetSize;
        usage.shared = counters.WorkingSetSize - counters.PrivateWorkingSetSize;
    }
    return usage;
};

} // memory
} // system
} // eboost

#endif

#endif
