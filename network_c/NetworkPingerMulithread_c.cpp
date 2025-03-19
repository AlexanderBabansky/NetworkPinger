#include "NetworkPingerMulithread_c.h"
#include "NetworkPingerMulithread.h"

void ping_c(int ip_count, const char *ips[], int timeoutMs,
                                     int triesCount, bool *results)
{
    std::vector<std::string> ips_vec;
    for (int a = 0; a < ip_count; ++a) {
        ips_vec.push_back(ips[a]);
    }
    auto results_vec = NetworkPingerMulithread::ping(ips_vec, timeoutMs, triesCount, 0);
    for (int a = 0; a < ip_count; ++a) {
        results[a] = results_vec[a];
    }
    return;
}
