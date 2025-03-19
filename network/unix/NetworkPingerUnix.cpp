#include "NetworkPinger.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/ip_icmp.h>
#include <sys/socket.h>
#include <time.h>
#include <errno.h>
#include <chrono>
#include "NMTimer.h"

#include "OnExit.h"

/**
* sudo setcap cap_net_raw=eip PATH
*/

unsigned short checksum(void *b, int len)
{
    unsigned short *buf = reinterpret_cast<unsigned short *>(b);
    unsigned int sum = 0;
    unsigned short result;

    for (sum = 0; len > 1; len -= 2) {
        sum += *buf++;
    }
    if (len == 1) {
        sum += *(unsigned char *)buf;
    }
    sum = (sum >> 16) + (sum & 0xFFFF);
    sum += (sum >> 16);
    result = ~sum;
    return result;
}

struct timeval milliseconds_to_timeval(long milliseconds)
{
    struct timeval tv
    {};

    tv.tv_sec = milliseconds / 1000;
    tv.tv_usec = (milliseconds % 1000) * 1000;

    return tv;
}

bool checkIcmpPacket(const struct icmphdr *icmp_header, uint16_t sequence)
{
    if (icmp_header->type != 0) {
        return false;
    }
    uint16_t pid16 = getpid();
    if (ntohs(icmp_header->un.echo.id) != pid16) {
        return false;
    }
    if (ntohs(icmp_header->un.echo.sequence) != sequence) {
        return false;
    }
    return true;
}

bool ping_device(const std::string &ipAddress, int timeoutMs, uint16_t sequence)
{
    int sockfd = 0;
    sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    if (sockfd < 0) {
        perror("Socket creation failed");
        return false;
    }
    OnExit socketClose([&]() { close(sockfd); });

    struct sockaddr_in addr_con
    {};
    addr_con.sin_family = AF_INET;
    addr_con.sin_port = 0;
    addr_con.sin_addr.s_addr = inet_addr(ipAddress.c_str());

    struct icmphdr icmp_hdr
    {};
    icmp_hdr.type = ICMP_ECHO;
    icmp_hdr.un.echo.id = htons(getpid());
    icmp_hdr.un.echo.sequence = htons(sequence);
    icmp_hdr.checksum = checksum(&icmp_hdr, sizeof(icmp_hdr));

    if (sendto(sockfd, &icmp_hdr, sizeof(icmp_hdr), 0, (struct sockaddr *)&addr_con,
               sizeof(addr_con))
        <= 0) {
        perror("Send failed");
        return false;
    }

    char buffer[1024]{};
    socklen_t addr_len = sizeof(addr_con);

    fd_set readfds{};
    FD_ZERO(&readfds);
    FD_SET(sockfd, &readfds);

    struct timeval timeout = milliseconds_to_timeval(timeoutMs);

    NMTimer timer;
    while (timer.elapsedMs() < timeoutMs) {
        int activity = select(sockfd + 1, &readfds, NULL, NULL, &timeout);
        if (activity < 0) {
            perror("Select error");
            return false;
        }
        if (activity == 0) {
            return false;
        }

        int bytesReceived = recvfrom(sockfd, buffer, sizeof(buffer), 0,
                                     (struct sockaddr *)&addr_con, &addr_len);
        if (bytesReceived <= 0) {
            perror("Receive failed");
            return false;
        }
        struct icmphdr *icmp_header = (struct icmphdr *)(buffer
                                                         + sizeof(
                                                             struct iphdr)); // Cast to ICMP header

        if (checkIcmpPacket(icmp_header, sequence)) {
            return true;
        }
    }
    return false;
}
