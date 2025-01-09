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
#include "service_utils.h"

LPCSTR SVCNAME = "Test";

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

#define SVCNAME TEXT("Test")
DWORD ServiceMain(HANDLE stopEvent)
{
    auto dbPath = service_utils::get_environment_variable("PINGER_DB");
    auto interval = service_utils::get_environment_variable_int("PINGER_INTERVAL");
    if (!dbPath || !interval) {
        service_utils::SvcReportMessage("No ENV variables", SVCNAME);
        return 1;
    }
    try {
        DBManager dbManager(dbPath->c_str());
        NMTimer timer;

        while (1) {
            auto wifis = dbManager.fetchWifi();
            auto pingResult = NetworkPingerMulithread::ping(collectIps(wifis), interval.value(),
                                                            stopEvent);

            time_t timestamp = time(NULL);
            dbManager.insertStatus(collectStatuses(wifis, pingResult), timestamp);

            if (timer.sleepUntilMsConditionAndStart(interval.value(), stopEvent) == false) {
                break;
            }
        }
    } catch (std::exception &e) {
        service_utils::SvcReportMessage(e.what(), SVCNAME);
        return 1;
    }
    return 0;
}
