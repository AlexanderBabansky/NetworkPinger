#pragma once
#include <vector>
#include <string>
#include <Windows.h>

namespace NetworkPingerMulithread {
std::vector<bool> ping(std::vector<std::string> ips, int timeoutMs, HANDLE interrupt, uint16_t startId = 0);
};