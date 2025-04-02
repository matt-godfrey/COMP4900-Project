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

//    std::vector<Packet> droppedPackets; // to store dropped packets for possible testing later

    std::mutex contextMutex;
    std::condition_variable packetAvailable;

//    FlowQueue::FlowQueue(int flowPriority, int periodMs, int budgetLimit)
//	  udpQueue_400(2, 5000, 5) -> can process max 5 packets per 5 sec time period

    SharedContext()
        : icmp_arpQueue(3, 5000, 3),
          udpQueue_400(2, 5000, 5),
          tcpQueue_80(2, 5000, 2),
          tcpQueue_443(2, 5000, 2),
          udpQueue(1, 5000, 2),
          defaultQueue(0, 5000, 2) {

        allQueues = { &icmp_arpQueue, &udpQueue_400, &tcpQueue_80,
                      &tcpQueue_443, &udpQueue, &defaultQueue };
    }
};

#endif /* INCLUDE_SHAREDCONTEXT_H_ */
