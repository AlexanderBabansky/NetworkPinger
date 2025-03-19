#pragma once
#include <vector>
#include <string>
#include <cstdint>

namespace NetworkPingerMulithread {
std::vector<bool> ping(std::vector<std::string> ips, int timeoutMs, int tries_count, uint16_t startId = 0);
};
