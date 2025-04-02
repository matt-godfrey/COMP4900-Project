/*
 * Packet.h
 *
 *  Created on: Mar. 30, 2025
 *      Author: matt
 */

#ifndef INCLUDE_PACKET_H_
#define INCLUDE_PACKET_H_

#include <string>
#include <chrono>

struct Packet {
    std::string srcIP;
    std::string protocol;
    int port = 0;
    std::string data;
    int priority = 0; // 0 = low, 1 = medium, 2 = med-high, 3 = high

    // when the packet was received
    std::chrono::system_clock::time_point timestamp;

    Packet() = default;

    Packet(const std::string& ip, const std::string& proto, int p, const std::string& d, int pri)
        : srcIP(ip), protocol(proto), port(p), data(d), priority(pri), timestamp(std::chrono::system_clock::now()) {}
};
#endif /* INCLUDE_PACKET_H_ */
