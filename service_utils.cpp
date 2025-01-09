#include "service_utils.h"
#include "service_utils.h"
#include "service_utils.h"
#include <strsafe.h>
#include <vector>

namespace service_utils {
VOID SvcReportMessage(LPCSTR message, LPCSTR srcName)
{
    DWORD SVC_ERROR = 0;

    HANDLE hEventSource = NULL;
    LPCTSTR lpszStrings[2];
    const int bufferSize = 200;
    TCHAR Buffer[bufferSize];

    hEventSource = RegisterEventSource(NULL, srcName);

    if (NULL != hEventSource) {
        StringCchPrintf(Buffer, bufferSize, TEXT("%s"), message);

        lpszStrings[0] = srcName;
        lpszStrings[1] = Buffer;

        ReportEvent(hEventSource,        // event log handle
                    EVENTLOG_ERROR_TYPE, // event type
                    0,                   // event category
                    SVC_ERROR,           // event identifier
                    NULL,                // no security identifier
                    2,                   // size of lpszStrings array
                    0,                   // no binary data
                    lpszStrings,         // array of strings
                    NULL);               // no binary data

        DeregisterEventSource(hEventSource);
    }
}

VOID SvcReportEvent(LPCSTR szFunction, LPCSTR srcName)
{
    HANDLE hEventSource;
    LPCTSTR lpszStrings[2];
    const int bufferSize = 200;
    TCHAR Buffer[bufferSize];
    DWORD SVC_ERROR = 0;

    hEventSource = RegisterEventSource(NULL, srcName);

    if (NULL != hEventSource) {
        StringCchPrintf(Buffer, bufferSize, TEXT("%s failed with %d"), szFunction, GetLastError());

        lpszStrings[0] = srcName;
        lpszStrings[1] = Buffer;

        ReportEvent(hEventSource,        // event log handle
                    EVENTLOG_ERROR_TYPE, // event type
                    0,                   // event category
                    SVC_ERROR,           // event identifier
                    NULL,                // no security identifier
                    2,                   // size of lpszStrings array
                    0,                   // no binary data
                    lpszStrings,         // array of strings
                    NULL);               // no binary data

        DeregisterEventSource(hEventSource);
    }
}

std::optional<std::string> get_environment_variable(const std::string &var_name)
{
    DWORD buffer_size = GetEnvironmentVariableA(var_name.c_str(), nullptr, 0);
    if (buffer_size == 0) {
        return {};
    }

    // Allocate a buffer and retrieve the value
    std::vector<char> buffer(buffer_size);
    GetEnvironmentVariableA(var_name.c_str(), buffer.data(), buffer_size);

    return std::string(buffer.data());
}

std::optional<int> get_environment_variable_int(const std::string &var_name)
{
    auto env = get_environment_variable(var_name);
    if (!env) {
        return {};
    }
    return atoi(env->c_str());
}

} // namespace service_utils
