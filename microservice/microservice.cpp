#include "rabbitmq-c/amqp.h"
#include <rabbitmq-c/tcp_socket.h>
#include "nlohmann/json.hpp"
#include "NetworkPingerMulithread.h"
#include <cstdio>
#include <map>
#include <thread>
#include <iostream>
#include "AMQPInstance.h"
#include "AMQPStructs.h"
#include <Windows.h>

using json = nlohmann::json;

namespace {} // namespace

int main()
{
    const char *hostname = "localhost";
    int port = 5672;
    const char *username = "guest";
    const char *password = "guest";
    const char *queueNameRequest = "ping_request";
    const char *queueNameResponse = "ping_response";

    auto instance = AMQPInstance::create(hostname, port, username, password);
    if (!instance) {
        return 1;
    }
    if (!instance->startConsumeQueue(queueNameRequest)) {
        return 1;
    }
    if (!instance->declareQueue(queueNameResponse)) {
        return 1;
    }

    std::thread netThread([&]() {
        while (1) {
            auto msg = instance->receiveMessage();
            if (msg.empty()) {
                return;
            }
            auto pingResponse = AMQPStructs::processJsonRequest(msg);
            if (pingResponse.empty()) {
                continue;
            }
            instance->sendString(queueNameResponse, pingResponse);
            printf("Got msg: %s\n", msg.c_str());
        }
    });
    int stop = 0;
    std::cin >> stop;
    instance->stop();
    netThread.join();
    return 0;
}
