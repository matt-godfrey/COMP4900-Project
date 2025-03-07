/*
 * PacketReceiver.h
 *
 *  Created on: Mar. 4, 2025
 *      Author: matt
 */

#ifndef PACKET_RECEIVER_H
#define PACKET_RECEIVER_H

#include <queue>
#include <mutex>
#include <condition_variable>
#include <netinet/in.h>

class PacketReceiver {
private:
    int sockfd;
    std::queue<std::string>& packetQueue; // packet queue in main class; might want to replace with priority queue
    std::mutex& queueMutex; // mutex for shared queue
    std::condition_variable& packetAvailable; // shared cond var
    bool running;

public:
    PacketReceiver(std::queue<std::string>& q, std::mutex& m, std::condition_variable& cv);
    ~PacketReceiver();
    void capturePackets();
};

#endif

