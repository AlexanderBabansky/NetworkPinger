#pragma once
#include <Windows.h>
#include <string>
#include <optional>

namespace service_utils {
VOID SvcReportMessage(LPCSTR message, LPCSTR srcName);
VOID SvcReportEvent(LPCSTR szFunction, LPCSTR srcName);

std::optional<std::string> get_environment_variable(const std::string &var_name);
std::optional<int> get_environment_variable_int(const std::string &var_name);
} // namespace service_utils
