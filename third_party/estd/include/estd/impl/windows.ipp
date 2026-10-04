#ifndef ESTD_SYSTEM_WINWDOWS_IPP
#define ESTD_SYSTEM_WINWDOWS_IPP

#ifdef _WIN32

#include "estd/system.hpp"

#include <windows.h>
#include <processthreadsapi.h>
#include <psapi.h>

::HANDLE g_hProcess{ ::GetCurrentProcess() };

::estd::cpu::usage
::estd::cpu::get_usage(
) {
	usage usage{};
	::FILETIME creation{};
	::FILETIME exit{};
	::FILETIME kernel{};
	::FILETIME user{};
	if (::GetProcessTimes(
			g_hProcess,
			&creation, &exit, &kernel, &user
		)) {
		usage.system = ((static_cast<
			size_t
		>(kernel.dwHighDateTime) << 32) | kernel.dwLowDateTime) * 100;
		usage.user = ((static_cast<
			size_t
		>(user.dwHighDateTime) << 32) | user.dwLowDateTime) * 100;
	}
	return usage;
};

::estd::memory::usage
::estd::memory::get_usage(
) {
	usage usage{};
	::PROCESS_MEMORY_COUNTERS_EX2 counters{};
	if (::GetProcessMemoryInfo(
			g_hProcess,
			reinterpret_cast<
				PROCESS_MEMORY_COUNTERS *
			>(&counters), sizeof(counters)
		)) {
		usage.anonymous = counters.PrivateWorkingSetSize;
		usage.shared = counters.WorkingSetSize - counters.PrivateWorkingSetSize;
	}
	return usage;
};

#endif

#endif
