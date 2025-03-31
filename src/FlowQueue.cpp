#include <FlowQueue.h>
#include "Packet.h"


FlowQueue::FlowQueue(int flowPriority, int periodMs, int budgetLimit)
    : flowPriority(flowPriority), periodMs(periodMs), budgetLimit(budgetLimit),
      budgetRemaining(budgetLimit), lastResetTime(std::chrono::steady_clock::now()) {}

void FlowQueue::enqueue(const Packet& packet) {
    std::lock_guard<std::mutex> lock(queueMutex);
    queue.push_back(packet);
}

//Packet FlowQueue::dequeue() {
//    std::lock_guard<std::mutex> lock(queueMutex);
//
//    if (queue.empty()) {
//        throw std::runtime_error("FlowQueue is empty");
//    }
//
//    Packet packet = queue.front();
//    queue.pop();
//    return packet;
//}

//Packet FlowQueue::dequeue() {
//    std::lock_guard<std::mutex> lock(queueMutex);
//    if (queue.empty())
//        throw std::runtime_error("Queue is empty");
//
//    // Find highest-priority packet
//    auto it = std::max_element(queue.begin(), queue.end(),
//        [](const Packet& a, const Packet& b) {
//            return a.priority < b.priority;
//        });
//
//    Packet highest = *it;
//    queue.erase(it);
//    return highest;
//}

Packet FlowQueue::dequeue() {
    if (queue.empty()) {
        throw std::runtime_error("Dequeue called on empty queue");
    }

    resetBudgetIfNeeded();
    if (budgetRemaining <= 0) {
        throw std::runtime_error("Rate limit exceeded");
    }

    Packet pkt = queue.front();
//    queue.pop();
    queue.erase(queue.begin());
    budgetRemaining--;
    return pkt;
}


bool FlowQueue::isEmpty() {
    std::lock_guard<std::mutex> lock(queueMutex);
    return queue.empty();
}

void FlowQueue::setPriority(int p) {
    std::lock_guard<std::mutex> lock(queueMutex);
	flowPriority = p;
}

int FlowQueue::getPriority() const {
	return flowPriority;
}

bool FlowQueue::canProcess() {
    resetBudgetIfNeeded();
    return !queue.empty() && budgetRemaining > 0;
}

void FlowQueue::resetBudgetIfNeeded() {
    auto now = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastResetTime);
    if (elapsed.count() >= periodMs) {
        budgetRemaining = budgetLimit;
        lastResetTime = now;
    }
}

//void FlowQueue::resetBudgetIfNeeded() {
//    auto now = std::chrono::steady_clock::now();
//    if (now - lastReset >= period) {
//        budgetRemaining = budget;
//        lastReset = now;
//    }
//}

