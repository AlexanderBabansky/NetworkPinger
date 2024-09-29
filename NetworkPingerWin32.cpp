#include <winsock2.h>
#include <iphlpapi.h>
#include <icmpapi.h>
#include <windows.h>
#include <iostream>
#include <vector>
#include <WS2tcpip.h>
#include <cstdlib>
#include "OnExit.h"

#include "NetworkPinger.h"

bool ping_device(const std::string &ipAddress, int timeoutMs, uint16_t sequence)
{
    IPAddr ip = inet_addr(ipAddress.c_str());
    if (ip == INADDR_NONE) {
        return false;
    }

    HANDLE hIcmpFile = IcmpCreateFile();
    if (hIcmpFile == INVALID_HANDLE_VALUE) {
        std::cerr << "Unable to open handle. IcmpCreateFile returned error: " << GetLastError()
                  << std::endl;
        return false;
    }
    OnExit icmpFileClean([&hIcmpFile]() { IcmpCloseHandle(hIcmpFile); });

    char sendData[32] = "Data Buffer";
    DWORD replySize = sizeof(ICMP_ECHO_REPLY) + sizeof(sendData) + 8;
    std::vector<char> replyBuffer(replySize);

    DWORD timeout = timeoutMs;

    DWORD echoResult = IcmpSendEcho(hIcmpFile, ip, sendData, sizeof(sendData), NULL,
                                    replyBuffer.data(), replySize, timeout);

    if (echoResult != 0) {
        PICMP_ECHO_REPLY echoReply = (PICMP_ECHO_REPLY)replyBuffer.data();
        if (echoReply->DataSize == sizeof(sendData)) {
            return true;
        }
    }

    return false;
}
