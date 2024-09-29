#pragma once
#include <string>

bool ping_device(const std::string &ip, int timeoutMs, uint16_t sequence);
