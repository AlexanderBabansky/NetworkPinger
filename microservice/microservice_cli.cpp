#include <cstdio>
#include <map>
#include <thread>
#include <iostream>
#include "AMQPInstance.h"
#include "AMQPStructs.h"
#include <signal.h>
#include <unistd.h>
#include <condition_variable>
#include "OnExit.h"
#include "NMLogger.h"

std::atomic_bool gShouldStop = false;
std::atomic_bool gErrorNetwork = false;
std::condition_variable gConditionStop;
std::mutex gShouldStopMutex;

void handle_signal(int sig)
{
    if (sig == SIGTERM || sig == SIGINT) {
        printf("Received SIGTERM, shutting down...\n");
        {
            std::lock_guard g(gShouldStopMutex);
            gShouldStop = true;
        }
        gConditionStop.notify_one();
    }
}

int main()
{
    struct sigaction action
    {};
    action.sa_handler = handle_signal;
    sigemptyset(&action.sa_mask);
    action.sa_flags = 0;
    sigaction(SIGTERM, &action, NULL);
    sigaction(SIGINT, &action, NULL);

    auto argsInfo = AMQPStructs::parseArgsInfoEnv();
    if (!argsInfo) {
        RE_LOG_ERROR("Failed to parse arguments");
        return 1;
    }

    /*
    * export AMQP_HOSTNAME="localhost"
    * export AMQP_PORT=5672
    * export AMQP_USERNAME="guest"
    * export AMQP_PASSWORD="guest"
    * export AMQP_QUEUE_REQUEST="ping_request"
    * export AMQP_QUEUE_RESPONSE="ping_response"
    */

    while (gShouldStop == false) {
        gErrorNetwork = false;
        auto instance = AMQPInstance::create(argsInfo->hostname, argsInfo->port,
                                             argsInfo->username, argsInfo->password);
        if (!instance) {
            continue;
        }
        if (!instance->startConsumeQueue(argsInfo->queueNameRequest)) {
            continue;
        }
        if (!instance->declareQueue(argsInfo->queueNameResponse)) {
            continue;
        }

        std::thread netThread([&]() {
            while (1) {
                auto msg = instance->receiveMessage();
                if (msg.empty()) {
                    {
                        std::lock_guard g(gShouldStopMutex);
                        gErrorNetwork = true;
                    }
                    gConditionStop.notify_one();
                    return;
                }
                auto pingResponse = AMQPStructs::processJsonRequest(msg);
                if (pingResponse.empty()) {
                    continue;
                }
                instance->sendString(argsInfo->queueNameResponse, pingResponse);
                printf("Got msg: %s\n", msg.c_str());
            }
        });
        printf("start Loop \n");
        while (gShouldStop == false && gErrorNetwork == false) {
            std::unique_lock g(gShouldStopMutex);
            gConditionStop.wait(g);
        }
        printf("Loop unlock\n");
        instance->stop();
        netThread.join();
    }
    return 0;
}
