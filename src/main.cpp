/*
 * main.cpp
 *
 *  Created on: Mar. 4, 2025
 *      Author: matt
 */

#include <FlowQueue.h>
#include "PacketReceiver.h"
#include "PacketProcessor.h"
#include "RateLimiter.h"
#include "DriverManager.h"
#include "NetworkTask.h"
#include "SharedContext.h"

#include <iostream>
#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>

int main() {

	SharedContext sharedContext;

//	std::queue<std::string> packetQueue;
//    std::mutex queueMutex;
//    std::condition_variable packetAvailable;

    std::cout << "Starting packet processor..." << std::endl;

    PacketReceiver receiver(sharedContext);
    PacketProcessor processor(sharedContext);

//    PacketProcessor processor(&packetQueue, &queueMutex, &packetAvailable);


    std::thread receiverThread(&PacketReceiver::capturePackets, &receiver);
    std::thread processorThread(&PacketProcessor::processPackets, &processor);

    receiverThread.join();
    processorThread.join();

    return 0;
}





