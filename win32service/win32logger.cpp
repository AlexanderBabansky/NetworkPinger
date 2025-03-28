#include <Windows.h>

#include "NMLogger.h"
#include <cstdio>
#include <cstdarg>
#include <string>
#include "win32logger.h"

extern LPCSTR SVCNAME;

namespace {

std::string stringifyFormat(const char *format, va_list args)
{
    char buffer[1024]{0};
    vsprintf_s(buffer, format, args);
    return buffer;
}

std::string getLastWin32Error()
{
    DWORD error_code = GetLastError();
    if (error_code == 0) {
        return "No error";
    }
    char *message_buffer = nullptr;
    DWORD size = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM
                                    | FORMAT_MESSAGE_IGNORE_INSERTS,
                                nullptr, error_code, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                                (LPSTR)&message_buffer, 0, nullptr);
    std::string message(message_buffer, size); // Store in std::string
    LocalFree(message_buffer);                 // Free allocated buffer
    return message;
}

} // namespace

void RE_LOG_ERROR(const char *format, va_list args)
{
    constexpr DWORD SVC_ERROR = 0;

    HANDLE hEventSource = NULL;
    hEventSource = RegisterEventSource(NULL, SVCNAME);
    if (!hEventSource) {
        return;
    }

    auto msg = stringifyFormat(format, args);

    LPCTSTR lpszStrings[2]{nullptr};
    lpszStrings[0] = SVCNAME;
    lpszStrings[1] = msg.data();

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

void RE_LOG_ERROR(const char *format, ...)
{
    va_list args;
    va_start(args, format);
    RE_LOG_ERROR(format, args);
    va_end(args);
}

void RE_LOG_WIN32_ERROR(const char *format, ...)
{
    auto lastError = getLastWin32Error();

    va_list args;
    va_start(args, format);
    auto msg = stringifyFormat(format, args);
    va_end(args);

    msg += ": %s";

    RE_LOG_ERROR(msg.c_str(), lastError.c_str());
    va_end(args);
}
