#pragma once
#include <vector>
#include <string>

namespace NetworkPingerMulithread {
std::vector<bool> ping(std::vector<std::string> ips, int timeoutMs, uint16_t startId = 0);
};