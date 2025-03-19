#pragma once

#ifdef _WIN32
#include <Windows.h>
#define SHARED_EXPORT __declspec(dllexport)
#else
#define SHARED_EXPORT
#endif

extern "C" SHARED_EXPORT void ping_c(int ip_count, const char *ips[], int timeoutMs,
                                     int triesCount, bool *results);
