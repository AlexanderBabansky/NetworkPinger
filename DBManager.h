#pragma once
#include "sqlite3.h"
#include <vector>
#include <string>

class DBManager
{
public:
    struct WiFi
    {
        int id = 0;
        std::string ip;
    };
    struct WiFiStatus
    {
        int id = 0;
        bool status = false;
    };
    DBManager(const char *filepath);
    ~DBManager();

    std::vector<WiFi> fetchWifi();
    void insertStatus(std::vector<WiFiStatus> statuses, int64_t timepoint);

private:
    sqlite3 *mDatabase = nullptr;
    sqlite3_stmt *mStmtFetch = nullptr;
    sqlite3_stmt *mStmtUpdate = nullptr;
};