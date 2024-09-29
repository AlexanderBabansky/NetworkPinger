#include <iostream>
#include <string>
#include <vector>
#include "NetworkPinger.h"
#include "future"
#include <cassert>

using namespace std;

const int TIMEOUT_OFFSET = 1;
const int ADDRESS_OFFSET = 2;

vector<string> parseAddresses(int argc, char **argv)
{
    int addressesToPingCount = argc - ADDRESS_OFFSET;
    vector<string> addressesToPing;
    addressesToPing.reserve(addressesToPingCount);

    for (int a = 0; a < addressesToPingCount; ++a) {
        addressesToPing.push_back(argv[a + ADDRESS_OFFSET]);
    }
    return addressesToPing;
}

int main(int argc, char **argv)
{
    if (argc <= ADDRESS_OFFSET) {
        cerr << "Wrong arguments" << endl;
        return 1;
    }

    const int timeoutMs = atoi(argv[TIMEOUT_OFFSET]);

    vector<string> addressesToPing = parseAddresses(argc, argv);
    vector<future<bool>> pingTasks;

    uint16_t sequence = 0;
    for (auto &pingAddress : addressesToPing) {
        ++sequence;
        pingTasks.push_back(async(launch::async, [pingAddress, timeoutMs, sequence]() {
            return ping_device(pingAddress, timeoutMs, sequence);
        }));
    }

    vector<bool> pingResults;
    pingResults.reserve(1);
    for (auto &t : pingTasks) {
        pingResults.push_back(t.get());
    }
    assert(pingResults.size() == pingTasks.size());

    for (auto r : pingResults) {
        if (r) {
            cout << "1 ";
        } else {
            cout << "0 ";
        }
    }
    return 0;
}
