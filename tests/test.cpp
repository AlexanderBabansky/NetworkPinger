#include "catch2/catch_all.hpp"
#include "NMTimer.h"
#include "NetworkPinger.h"
#include <future>
#include "NetworkPingerMulithread.h"
#include "NetworkPingerMulithread_c.h"
TEST_CASE("Sleep")
{
    auto lambda = [](const std::string pingAddress) {
        while (1) {
            const char *pingArr[2]{"0.0.0.0", "127.0.0.1"};
            bool results[2]{false};
            ping_c(2, pingArr, 1000, 3, results);
            printf("-------------\n");
            for (bool res : results) {
                printf("%d\n", res);
            }
            Sleep(1000);
        }
    };

    const std::string pingAddress1 = "192.168.88.244";
    const std::string pingAddress2 = "192.168.88.1";

    auto t1 = std::async(lambda, pingAddress1);

    t1.wait();
}