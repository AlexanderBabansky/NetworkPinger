#pragma once
#include <string>
#include <cstdint>

namespace NetworkPinger {
bool ping_device(const std::string &ip, int timeoutMs, uint16_t pid, uint16_t sequence);
}
