/*
 * SharedContext.h
 *
 *  Created on: Mar. 26, 2025
 *      Author: matt
 */

#ifndef INCLUDE_SHAREDCONTEXT_H_
#define INCLUDE_SHAREDCONTEXT_H_

#include "FlowQueue.h"
#include <condition_variable>
#include <mutex>

struct SharedContext {
    //FlowQueue arpQueue;
    FlowQueue icmp_arpQueue; // network health; priority: high = 3
    FlowQueue udpQueue_400; // custom telemetry data; priority: medium-high = 2
    FlowQueue tcpQueue_80; // web traffic/logging; priority: low = 1
    FlowQueue tcpQueue_443; // secure communication/OTA updates; priority: medium = 1
    FlowQueue udpQueue; // default UDP queue; priority low = 0
    FlowQueue defaultQueue; // unknown traffic; prioroty low = 0


    std::vector<FlowQueue*> allQueues;

    std::mutex contextMutex;
    std::condition_variable packetAvailable;

    SharedContext() {
        allQueues = {
            &icmp_arpQueue,
            &udpQueue_400,
            &tcpQueue_80,
            &tcpQueue_443,
            &udpQueue,
            &defaultQueue
        };
    }
};

#endif /* INCLUDE_SHAREDCONTEXT_H_ */
