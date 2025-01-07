#include "DBManager.h"
#include <stdexcept>

DBManager::DBManager(const char *filepath)
{
    if (sqlite3_open_v2(filepath, &mDatabase, SQLITE_OPEN_READWRITE, NULL) != SQLITE_OK) {
        DBManager::~DBManager();
        throw std::runtime_error("Could not open database");
    }
    if (sqlite3_busy_timeout(mDatabase, 5000) != SQLITE_OK) {
        throw std::runtime_error("Could not set busy timeout");
    }

    const char *dbName = sqlite3_db_name(mDatabase, 0);
    if (!dbName) {
        DBManager::~DBManager();
        throw std::runtime_error("Could not open database");
    }
    if (sqlite3_db_readonly(mDatabase, dbName)) {
        DBManager::~DBManager();
        throw std::runtime_error("Database is read only");
    }
    if (sqlite3_prepare_v3(mDatabase, "SELECT ID, ip FROM wifi;", -1, SQLITE_PREPARE_PERSISTENT,
                           &mStmtFetch, nullptr)
        != SQLITE_OK) {
        DBManager::~DBManager();
        throw std::runtime_error("Failed to prepare STMT");
    }
    if (sqlite3_prepare_v3(mDatabase,
                           "INSERT INTO wifi_status (wifi_ID, timepoint, status) VALUES(?, ?, ?);",
                           -1, SQLITE_PREPARE_PERSISTENT, &mStmtUpdate, nullptr)
        != SQLITE_OK) {
        DBManager::~DBManager();
        throw std::runtime_error("Failed to prepare STMT");
    }
}

DBManager::~DBManager()
{
    sqlite3_finalize(mStmtFetch);
    sqlite3_finalize(mStmtUpdate);
    sqlite3_close_v2(mDatabase);
}

std::vector<DBManager::WiFi> DBManager::fetchWifi()
{
    std::vector<DBManager::WiFi> result;
    int rc = 0;
    while ((rc = sqlite3_step(mStmtFetch)) == SQLITE_ROW) {
        DBManager::WiFi wifi{};
        wifi.id = sqlite3_column_int(mStmtFetch, 0);
        wifi.ip = (const char *)sqlite3_column_text(mStmtFetch, 1);
        result.push_back(std::move(wifi));
    }
    if (sqlite3_reset(mStmtFetch) != SQLITE_OK) {
        throw std::runtime_error("Failed to reset");
    }
    if (rc != SQLITE_DONE) {
        throw std::runtime_error("Failed to fetch");
    }
    return result;
}

void DBManager::insertStatus(std::vector<DBManager::WiFiStatus> statuses, int64_t timepoint)
{
    int rc = 0;

    for (auto &s : statuses) {
        sqlite3_bind_int(mStmtUpdate, 1, s.id);
        sqlite3_bind_int64(mStmtUpdate, 2, timepoint);
        sqlite3_bind_int(mStmtUpdate, 3, s.status ? 1 : 0);
        rc = sqlite3_step(mStmtUpdate);
        {
            int rc2 = sqlite3_reset(mStmtUpdate);
            if (rc2 != SQLITE_OK) {
                throw std::runtime_error("Failed to reset");
            }
        }
        if (rc != SQLITE_DONE) {
            throw std::runtime_error("Failed to insert row");
        }
    }
}
