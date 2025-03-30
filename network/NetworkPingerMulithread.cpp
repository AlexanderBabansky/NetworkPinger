#include "NetworkPingerMulithread.h"
#include "NetworkPinger.h"
#include <future>
#include <cassert>

std::vector<bool> NetworkPingerMulithread::ping(std::vector<std::string> addressesToPing,
                                                int timeoutMs, int tries_count)
{
    std::vector<std::future<bool>> pingTasks;

    uint16_t sequence = 0;
    for (auto &pingAddress : addressesToPing) {
        sequence += tries_count;
        pingTasks.push_back(
            std::async(std::launch::async, [pingAddress, timeoutMs, sequence, tries_count]() {
                bool status = false;
                int try_id = 0;
                while (status == false && try_id < tries_count) {
                    uint16_t pid = 0;
#ifndef WIN32
                    pid = getpid();
#endif // ! WIN32

                    status = NetworkPinger::ping_device(pingAddress, timeoutMs, pid, sequence);
                    ++try_id;
                }
                return status;
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
