#pragma once
#include <map>
#include <string>
#include <optional>

namespace AMQPStructs {

struct PingRequest
{
    int timeout = 0;
    int triesCount = 0;
    std::map<int, std::string> hosts;

    static std::optional<PingRequest> parseFromJsonString(const std::string &jsonString);
};

struct PingResponse
{
    std::map<int, bool> status;
    std::string toJsonString() const;
};

PingResponse getPingResponseFromRequest(const PingRequest &request);

std::string processJsonRequest(const std::string &requestJson);

struct ArgsInfo
{
    const char *hostname = nullptr;
    int port = 0;
    const char *username = nullptr;
    const char *password = nullptr;
    const char *queueNameRequest = nullptr;
    const char *queueNameResponse = nullptr;
};

std::optional<ArgsInfo> parseArgsInfo(int dwArgc, char **lpszArgv);
std::optional<ArgsInfo> parseArgsInfoEnv();

} // namespace AMQPStructs
