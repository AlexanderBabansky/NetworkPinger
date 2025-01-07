#include "sqlite3.h"
#include <stdexcept>
#include "DBManager.h"
#include "NetworkPingerMulithread.h"
#include <cstdio>
#include <ctime>
#include "NMTimer.h"
#include <Windows.h>
#include <strsafe.h>
#include <stdio.h>

namespace {
std::vector<std::string> collectIps(const std::vector<DBManager::WiFi> &wifis)
{
    std::vector<std::string> result;
    for (auto &w : wifis) {
        result.push_back(w.ip);
    }
    return result;
}

std::vector<DBManager::WiFiStatus> collectStatuses(const std::vector<DBManager::WiFi> &wifis,
                                                   const std::vector<bool> statuses)
{
    std::vector<DBManager::WiFiStatus> result;
    result.reserve(wifis.size());
    for (int a = 0; a < wifis.size(); ++a) {
        DBManager::WiFiStatus add;
        add.id = wifis[a].id;
        add.status = statuses[a];
        result.push_back(add);
    }
    return result;
}

} // namespace

VOID SvcReportEvent(LPCSTR szFunction, LPCSTR srcName)
{
    DWORD SVC_ERROR = 0;

    HANDLE hEventSource = NULL;
    LPCTSTR lpszStrings[2];
    TCHAR Buffer[80];

    hEventSource = RegisterEventSource(NULL, srcName);

    if (NULL != hEventSource) {
        StringCchPrintf(Buffer, 80, TEXT("%s"), szFunction);

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

VOID WINAPI SvcMain(DWORD, LPTSTR *) { SvcReportEvent("Hello", "SvcMain"); }

#define SVCNAME TEXT("Test")

int main()
{
    //SvcReportEvent("Hello", "Main");
    SERVICE_TABLE_ENTRY DispatchTable[] = {{SVCNAME, (LPSERVICE_MAIN_FUNCTION)SvcMain},
                                           {NULL, NULL}};
    if (!StartServiceCtrlDispatcher(DispatchTable)) {
        SvcReportEvent(TEXT("StartServiceCtrlDispatcher"));
    } 

    return 0;
    const char *filename = "db.db";
    int timeoutMs = 1000;

    DBManager dbManager(filename);

    NMTimer timer;
    while (1) {
        auto wifis = dbManager.fetchWifi();
        auto pingResult = NetworkPingerMulithread::ping(collectIps(wifis), timeoutMs);

        printf("------------\n");
        for (int a = 0; a < pingResult.size(); ++a) {
            printf("%s %d\n", wifis[a].ip.c_str(), pingResult[a] ? 1 : 0);
        }

        time_t timestamp = time(NULL);
        dbManager.insertStatus(collectStatuses(wifis, pingResult), timestamp);
        timer.sleepUntilMsAndStart(timeoutMs);
        SvcReportEvent("Hello", SVCNAME);
    }

    return 0;
}
