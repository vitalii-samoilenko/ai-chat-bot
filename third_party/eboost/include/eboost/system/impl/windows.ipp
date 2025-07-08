#ifndef EBOOST_SYSTEM_WINWDOWS_IPP
#define EBOOST_SYSTEM_WINWDOWS_IPP

#include "eboost/system/memory.hpp"

#ifdef _WIN32

#include <windows.h>
#include <processthreadsapi.h>
#include <psapi.h>

namespace eboost {
namespace system {
namespace memory {

usage get_usage() {
    ::PROCESS_MEMORY_COUNTERS_EX2 counters{};
    ::GetProcessMemoryInfo(::GetCurrentProcess(),
        reinterpret_cast<PROCESS_MEMORY_COUNTERS*>(&counters), sizeof(counters));
    return {
        counters.WorkingSetSize,
        counters.WorkingSetSize - counters.PrivateWorkingSetSize
    };
};

} // memory
} // system
} // eboost

#endif

#endif
