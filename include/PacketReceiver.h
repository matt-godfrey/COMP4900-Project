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
#include "SharedContext.h"

class PacketReceiver {
private:
    int sockfd;
    SharedContext& context;
    bool running;

public:
    PacketReceiver(SharedContext& ctx);
    ~PacketReceiver();
    void capturePackets();
    int assignPriority(const std::string& proto, int port);
};

#endif

