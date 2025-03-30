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

std::optional<ArgsInfo> AMQPStructs::parseArgsInfo(int dwArgc, char **lpszArgv)
{
    if (dwArgc != 7) {
        return {};
    }
    ArgsInfo result{};
    result.hostname = lpszArgv[1];
    result.port = utils::strToInt(lpszArgv[2]);
    result.username = lpszArgv[3];
    result.password = lpszArgv[4];
    result.queueNameRequest = lpszArgv[5];
    result.queueNameResponse = lpszArgv[6];
    return result;
}

std::optional<ArgsInfo> AMQPStructs::parseArgsInfoEnv()
{
    ArgsInfo result{};
    result.hostname = getenv("AMQP_HOSTNAME");
    if (!result.hostname) {
        return {};
    }
    result.port = utils::strToInt(getenv("AMQP_PORT"));
    if (result.port == 0) {
        return {};
    }
    result.username = getenv("AMQP_USERNAME");
    result.password = getenv("AMQP_PASSWORD");
    result.queueNameRequest = getenv("AMQP_QUEUE_REQUEST");
    result.queueNameResponse = getenv("AMQP_QUEUE_RESPONSE");

    return result;
}
