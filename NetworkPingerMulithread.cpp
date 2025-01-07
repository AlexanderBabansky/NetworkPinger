#include "NetworkPingerMulithread.h"
#include "NetworkPinger.h"
#include <future>
#include <cassert>

std::vector<bool> NetworkPingerMulithread::ping(std::vector<std::string> addressesToPing,
                                                int timeoutMs, uint16_t startId)
{
    std::vector<std::future<bool>> pingTasks;

    uint16_t sequence = startId;
    for (auto &pingAddress : addressesToPing) {
        ++sequence;
        pingTasks.push_back(std::async(std::launch::async, [pingAddress, timeoutMs, sequence]() {
            return ping_device(pingAddress, timeoutMs, sequence);
        }));
    }
    std::vector<bool> pingResults;
    pingResults.reserve(1);
    for (auto &t : pingTasks) {
        pingResults.push_back(t.get());
    }
    assert(pingResults.size() == pingTasks.size());
    return pingResults;
}