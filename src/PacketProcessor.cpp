#include "PacketProcessor.h"
#include <iostream>
#include <thread>
#include <iomanip>
#include <ctime>


PacketProcessor::PacketProcessor(SharedContext& ctx)
    : context(ctx), running(true) {}

//PacketProcessor(std::queue<std::string>* q, std::mutex* m, std::condition_variable* cv) {
//    packetQueue = q;
//    queueMutex = m;
//    packetAvailable = cv;
//    running = true;
//}


void PacketProcessor::processPackets() {
    while (running) {
        std::unique_lock<std::mutex> lock(context.contextMutex);

        // if all queues are empty, wait
        // else, wake up and process first non-empty flow queue
        context.packetAvailable.wait(lock, [this]() {
            return !context.icmp_arpQueue.isEmpty() ||
                   !context.udpQueue_400.isEmpty() ||
                   !context.tcpQueue_80.isEmpty() ||
                   !context.tcpQueue_443.isEmpty() ||
                   !context.udpQueue.isEmpty() ||
                   !context.defaultQueue.isEmpty();
        });

        FlowQueue* selectedQueue = nullptr;
        int maxPriority = -1;

        // Select the queue with the highest flow priority that can process packets
        for (auto* q : context.allQueues) {
            if (!q->isEmpty() && q->getPriority() > maxPriority && q->canProcess()) {
                selectedQueue = q;
                maxPriority = q->getPriority();
            }
        }

        // skip if no packets found
        if (!selectedQueue || selectedQueue->isEmpty()) {
            continue;
        }

//        // Dequeue the highest priority packet
//        Packet pkt = selectedQueue->dequeue();
        Packet pkt;
        bool found = false;

//        try {
//            pkt = selectedQueue->dequeue();
//            found = true;
//        } catch (const std::runtime_error& e) {
//            std::cerr << "[WARN] " << e.what() << std::endl;
//            continue;
//        }
//        try {
//        	// grab first packet without removing
//            pkt = selectedQueue->peek();
//            // if can't process queue, drop packet and store for later
//            if (!selectedQueue->canProcess()) {
//                selectedQueue->addDroppedPacket(pkt);
//                throw std::runtime_error("Rate limit exceeded");
//            }
//
//            pkt = selectedQueue->dequeue();
//            found = true;
//
//        } catch (const std::runtime_error& e) {
//            std::cerr << "[WARN] " << e.what() << std::endl;
//            continue;
//        }

        try {
            pkt = selectedQueue->dequeue();
            found = true;
        } catch (const std::runtime_error& e) {
            std::cerr << "[WARN] " << e.what() << std::endl;
            continue;
        }

        // unlock mutex
        lock.unlock();

        auto processTime = std::chrono::system_clock::now();
        std::time_t timeT = std::chrono::system_clock::to_time_t(processTime);

        if (found) {
            std::cout << "-------------------------------------------------------------------------------------------------------------" << std::endl;
            std::cout << "[" << std::put_time(std::localtime(&timeT), "%Y-%m-%d %H:%M:%S") << "] "
					  << "	Processing packet from " << pkt.srcIP
                      << " [PROTO=" << pkt.protocol << ", PORT=" << pkt.port
                      << ", PRIORITY=" << pkt.priority << "]" << ": "
                      << pkt.data << std::endl;
        }

    }
}



//void PacketProcessor::updatePriority() {
//	std::cout << "Updating priority.." << std::endl;
//}
