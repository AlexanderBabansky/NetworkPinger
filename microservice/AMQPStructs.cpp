#include "AMQPStructs.h"
#include "nlohmann/json.hpp"
#include "utils.h"
#include "NetworkPingerMulithread.h"

using namespace AMQPStructs;
using json = nlohmann::json;

std::optional<PingRequest> PingRequest::parseFromJsonString(const std::string &jsonString)
{
    PingRequest result{};
    try {
        json data = json::parse(jsonString);
        result.timeout = data.at("timeout").get<int>();
        result.triesCount = data.at("tries").get<int>();
        for (auto &h : data.at("hosts").items()) {
            auto ip_address = h.value().get<std::string>();
            int key = atoi(h.key().c_str());
            result.hosts[key] = ip_address;
        }
        return result;
    } catch (...) {
        return {};
    }
}

std::string PingResponse::toJsonString() const
{
    json jsonData;
    for (auto &h : status) {
        jsonData[utils::intToStr(h.first)] = h.second;
    }
    return jsonData.dump();
}

PingResponse AMQPStructs::getPingResponseFromRequest(const PingRequest &request)
{
    std::vector<std::string> hosts;
    for (auto &h : request.hosts) {
        hosts.push_back(h.second);
    }
    auto result = NetworkPingerMulithread::ping(hosts, request.timeout, request.triesCount);
    PingResponse response{};
    int a = 0;
    for (auto &h : request.hosts) {
        response.status[h.first] = result.at(a++);
    }
    return response;
}

std::string AMQPStructs::processJsonRequest(const std::string &msg)
{
    auto pingRequest = AMQPStructs::PingRequest::parseFromJsonString(msg);
    if (!pingRequest) {
        return {};
    }
    auto pingResponse = getPingResponseFromRequest(pingRequest.value());
    return pingResponse.toJsonString();
}
