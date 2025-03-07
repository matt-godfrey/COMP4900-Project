#include "PacketProcessor.h"
#include <iostream>
#include <thread>

PacketProcessor::PacketProcessor(std::queue<std::string>& q, std::mutex& m, std::condition_variable& cv)
    : packetQueue(q), queueMutex(m), packetAvailable(cv), running(true) {}

//PacketProcessor(std::queue<std::string>* q, std::mutex* m, std::condition_variable* cv) {
//    packetQueue = q;
//    queueMutex = m;
//    packetAvailable = cv;
//    running = true;
//}

void PacketProcessor::processPackets() {
    while (running) {
    	// lock queue
        std::unique_lock<std::mutex> lock(queueMutex);
//        packetAvailable.wait(lock, [this]() { return !packetQueue.empty(); });
        while (packetQueue.empty()) {
            packetAvailable.wait(lock);
        }


        std::string packet = packetQueue.front();
        packetQueue.pop();
        // unlock queue after processing
        lock.unlock();

        std::cout << "Processing packet: " << packet << std::endl;
    }
}

//void PacketProcessor::updatePriority() {
//	std::cout << "Updating priority.." << std::endl;
//}
