/*
 * main.cpp
 *
 *  Created on: Mar. 4, 2025
 *      Author: matt
 */

#include "PacketReceiver.h"
#include "PacketQueue.h"
#include "PacketProcessor.h"
#include "RateLimiter.h"
#include "DriverManager.h"
#include "NetworkTask.h"

#include <iostream>
#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>

int main() {
    std::queue<std::string> packetQueue;
    std::mutex queueMutex;
    std::condition_variable packetAvailable;

    std::cout << "Starting packet processor..." << std::endl;

    PacketReceiver receiver(packetQueue, queueMutex, packetAvailable);
    PacketProcessor processor(packetQueue, queueMutex, packetAvailable);

//    PacketProcessor processor(&packetQueue, &queueMutex, &packetAvailable);


    std::thread receiverThread(&PacketReceiver::capturePackets, &receiver);
    std::thread processorThread(&PacketProcessor::processPackets, &processor);

    receiverThread.join();
    processorThread.join();

    return 0;
}





