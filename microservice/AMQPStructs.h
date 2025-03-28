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

} // namespace AMQPStructs
