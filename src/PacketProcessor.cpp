#include "PacketProcessor.h"
#include <iostream>
#include <thread>

PacketProcessor::PacketProcessor(SharedContext& ctx)
    : context(ctx), running(true) {}

//PacketProcessor(std::queue<std::string>* q, std::mutex* m, std::condition_variable* cv) {
//    packetQueue = q;
//    queueMutex = m;
//    packetAvailable = cv;
//    running = true;
//}



//void PacketProcessor::processPackets() {
//    while (running) {
//        std::unique_lock<std::mutex> lock(context.contextMutex);
//        context.packetAvailable.wait(lock, [this]() {
//            return !context.icmp_arpQueue.isEmpty() ||
//                   !context.udpQueue_400.isEmpty() ||
//                   !context.tcpQueue_80.isEmpty() ||
//                   !context.tcpQueue_443.isEmpty();
//        });
//
//        Packet pkt;
//        bool found = false;
//
//        try {
//            if (!context.icmp_arpQueue.isEmpty()) {
//                pkt = context.icmp_arpQueue.dequeue();
//                found = true;
//            } else if (!context.udpQueue_400.isEmpty()) {
//                pkt = context.udpQueue_400.dequeue();
//                found = true;
//            } else if (!context.tcpQueue_80.isEmpty()) {
//                pkt = context.tcpQueue_80.dequeue();
//                found = true;
//            } else if (!context.tcpQueue_443.isEmpty()) {
//                pkt = context.tcpQueue_443.dequeue();
//                found = true;
//            }
//        } catch (const std::runtime_error& e) {
//            std::cerr << "[WARN] Dequeue error: " << e.what() << std::endl;
//            continue;
//        }
//
//        lock.unlock();
//
//        if (found) {
//            std::cout << "Processing packet from " << pkt.srcIP
//                      << " [PROTO=" << pkt.protocol << ", PORT=" << pkt.port
//                      << ", PRIORITY=" << pkt.priority << "]: "
//                      << pkt.data << std::endl;
//        }
//    }
//}


//void PacketProcessor::processPackets() {
//    while (running) {
//        std::unique_lock<std::mutex> lock(context.contextMutex);
//        context.packetAvailable.wait(lock, [this]() {
//            return !context.icmp_arpQueue.isEmpty() ||
//                   !context.udpQueue_400.isEmpty() ||
//                   !context.tcpQueue_80.isEmpty() ||
//                   !context.tcpQueue_443.isEmpty() ||
//				   !context.udpQueue.isEmpty() ||
//				   !context.defaultQueue.isEmpty();
//        });
//
//        FlowQueue* selectedQueue = nullptr;
//        int maxFlowPriority = -1;
//
//        // select non-empty flow queue with the highest flow priority
//        auto& q1 = context.icmp_arpQueue;
//        auto& q2 = context.udpQueue_400;
//        auto& q3 = context.tcpQueue_80;
//        auto& q4 = context.tcpQueue_443;
//        auto& q5 = context.udpQueue;
//        auto& q6 = context.defaultQueue;
//
//        if (!q1.isEmpty() && q1.getPriority() > maxFlowPriority) {
//            selectedQueue = &q1;
//            maxFlowPriority = q1.getPriority();
//        }
//        if (!q2.isEmpty() && q2.getPriority() > maxFlowPriority) {
//            selectedQueue = &q2;
//            maxFlowPriority = q2.getPriority();
//        }
//        if (!q3.isEmpty() && q3.getPriority() > maxFlowPriority) {
//            selectedQueue = &q3;
//            maxFlowPriority = q3.getPriority();
//        }
//        if (!q4.isEmpty() && q4.getPriority() > maxFlowPriority) {
//            selectedQueue = &q4;
//            maxFlowPriority = q4.getPriority();
//        }
//        if (!q5.isEmpty() && q4.getPriority() > maxFlowPriority) {
//            selectedQueue = &q5;
//            maxFlowPriority = q5.getPriority();
//        }
//        if (!q6.isEmpty() && q6.getPriority() > maxFlowPriority) {
//            selectedQueue = &q6;
//            maxFlowPriority = q6.getPriority();
//        }
//
//        // Dequeue packet
//        Packet pkt;
//        if (selectedQueue && !selectedQueue->isEmpty()) {
//            pkt = selectedQueue->dequeue();
//        }
//
//        lock.unlock();
//
//        std::cout << "Processing packet from " << pkt.srcIP
//                  << " [PROTO=" << pkt.protocol << ", PORT=" << pkt.port
//                  << ", PRIORITY=" << pkt.priority << "]: "
//                  << pkt.data << std::endl;
//    }
//}

void PacketProcessor::processPackets() {
    while (running) {
        std::unique_lock<std::mutex> lock(context.contextMutex);

        // Wait until there's something to process
        context.packetAvailable.wait(lock, [this]() {
            return !context.icmp_arpQueue.isEmpty() ||
                   !context.udpQueue_400.isEmpty() ||
                   !context.tcpQueue_80.isEmpty() ||
                   !context.tcpQueue_443.isEmpty() ||
                   !context.udpQueue.isEmpty() ||
                   !context.defaultQueue.isEmpty();
        });

//        std::vector<FlowQueue*> queues = {
//            &context.icmp_arpQueue,
//            &context.udpQueue_400,
//            &context.tcpQueue_80,
//            &context.tcpQueue_443,
//            &context.udpQueue,
//            &context.defaultQueue
//        };

        FlowQueue* selectedQueue = nullptr;
        int maxPriority = -1;

        // Select the queue with the highest flow priority
        for (auto* q : context.allQueues) {
            if (!q->isEmpty() && q->getPriority() > maxPriority) {
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
        try {
            pkt = selectedQueue->dequeue();
            found = true;
        } catch (const std::runtime_error& e) {
            std::cerr << "[WARN] " << e.what() << std::endl;
            continue;
        }


        lock.unlock();

        auto timeReceived = std::chrono::duration_cast<std::chrono::milliseconds>(
            pkt.timestamp.time_since_epoch()).count();

        if (found) {
            std::cout << "Processing packet from " << pkt.srcIP
                      << " [PROTO=" << pkt.protocol << ", PORT=" << pkt.port
                      << ", PRIORITY=" << pkt.priority << "] @ " << timeReceived << "ms: "
                      << pkt.data << std::endl;
        }

//        // simulate processing packet
//        std::cout << "Processing packet from " << pkt.srcIP
//                  << " [PROTO=" << pkt.protocol << ", PORT=" << pkt.port
//                  << ", PRIORITY=" << pkt.priority << "]: "
//                  << pkt.data << std::endl;
    }
}



//void PacketProcessor::updatePriority() {
//	std::cout << "Updating priority.." << std::endl;
//}
