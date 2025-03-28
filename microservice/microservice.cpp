#include <Windows.h>
#include "utils.h"
#include <optional>
#include "AMQPInstance.h"
#include "AMQPStructs.h"
#include <thread>
#include "OnExit.h"

#include "NMLogger.h"
#include "win32logger.h"

LPCSTR SVCNAME = "NetworkPingerService";

namespace {
struct ArgsInfo
{
    const char *hostname = nullptr;
    int port = 0;
    const char *username = nullptr;
    const char *password = nullptr;
    const char *queueNameRequest = nullptr;
    const char *queueNameResponse = nullptr;
};

std::optional<ArgsInfo> parseArgsInfo(int dwArgc, char **lpszArgv)
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

void networkThread(HANDLE networkStoppedEvent, std::shared_ptr<AMQPInstance> instance,
                   const char *queueNameResponse)
{
    OnExit onExit([networkStoppedEvent]() { SetEvent(networkStoppedEvent); });

    while (1) {
        auto msg = instance->receiveMessage();
        if (msg.empty()) {
            return;
        }
        auto pingResponse = AMQPStructs::processJsonRequest(msg);
        if (pingResponse.empty()) {
            continue;
        }
        if (!instance->sendString(queueNameResponse, pingResponse)) {
            return;
        }
    }
}
} // namespace

DWORD ServiceMain(HANDLE stopEvent, DWORD dwArgc, LPTSTR *lpszArgv)
{
    auto argsInfo = parseArgsInfo(dwArgc, lpszArgv);
    if (!argsInfo) {
        RE_LOG_ERROR("Bad arguments");
        return 1;
    }

    HANDLE networkStoppedEvent = CreateEvent(NULL,  // default security attributes
                                             TRUE,  // manual reset event
                                             FALSE, // not signaled
                                             NULL); // no name
    if (!networkStoppedEvent) {
        RE_LOG_WIN32_ERROR("Failed to CreateEvent");
        return 1;
    }

    auto instance = AMQPInstance::create(argsInfo->hostname, argsInfo->port, argsInfo->username,
                                         argsInfo->password);
    if (!instance) {
        return 1;
    }
    if (!instance->declareQueue(argsInfo->queueNameResponse)) {
        return 1;
    }
    if (!instance->startConsumeQueue(argsInfo->queueNameRequest)) {
        return 1;
    }
    std::thread netThread(&networkThread, networkStoppedEvent, instance,
                          argsInfo->queueNameResponse);

    HANDLE waitHandles[2]{stopEvent, networkStoppedEvent};
    WaitForMultipleObjects(2, waitHandles, FALSE, INFINITE);
    instance->stop();
    netThread.join();
    return 0;
}
