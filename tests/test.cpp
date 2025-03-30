#include "catch2/catch_all.hpp"
#include <future>
#include "NetworkPingerMulithread.h"

TEST_CASE("Ping")
{
    std::vector<std::string> pingArr{"127.0.0.1", "128.0.0.1"};
    auto results = NetworkPingerMulithread::ping(pingArr, 10, 3);
    REQUIRE(results.size() == 2);
    CHECK(results[0] == true);
    CHECK(results[1] == false);
}
