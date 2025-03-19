#include "catch2/catch_all.hpp"
#include <future>
#include "NetworkPingerMulithread_c.h"

TEST_CASE("Ping")
{
    const char *pingArr[2]{"128.0.0.1", "127.0.0.1"};
    bool results[2]{false};
    ping_c(2, pingArr, 1000, 3, results);
    
    CHECK(results[0] == false);
    CHECK(results[1] == true);
}
