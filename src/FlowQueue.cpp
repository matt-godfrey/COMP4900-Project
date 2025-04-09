#include <FlowQueue.h>
#include "Packet.h"
#include <iostream>


FlowQueue::FlowQueue(int flowPriority, int periodMs, int budgetLimit)
    : flowPriority(flowPriority), periodMs(periodMs), budgetLimit(budgetLimit),
      budgetRemaining(budgetLimit), lastResetTime(std::chrono::steady_clock::now()) {}


void FlowQueue::enqueue(const Packet& packet) {
    std::lock_guard<std::mutex> lock(queueMutex);
    resetBudgetIfNeeded();

    if (budgetRemaining <= 0) {
        std::cout << "[RATE LIMIT] FlowQueue over budget. Dropping packet.\n";
        return;
    }

    queue.push_back(packet);
    budgetRemaining--;
}


Packet FlowQueue::dequeue() {
    std::lock_guard<std::mutex> lock(queueMutex);

    // this should never happen
    if (queue.empty())
        throw std::runtime_error("Queue is empty");

    // Find highest-priority packet
    auto it = std::max_element(queue.begin(), queue.end(),
        [](const Packet& a, const Packet& b) {
            return a.priority < b.priority;
        });

    // retrieve max and remove it
    Packet highest = *it;
    queue.erase(it);
    return highest;
}

Packet FlowQueue::peek() const {
    if (queue.empty()) {
        throw std::runtime_error("Peek called on empty queue");
    }
    return queue.front();
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
    if (queue.empty()) return false;

    if (budgetRemaining <= 0) {
//        std::cout << "[RATE LIMIT] Queue budget exceeded. Dropping packet.\n";
        return false;
    }
    return true;
}

void FlowQueue::resetBudgetIfNeeded() {
	// get current time
    auto now = std::chrono::steady_clock::now();
    // calculate how much time since last budget reset
    // converts to milliseconds
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastResetTime);

    // elapsed.count(): num milliseconds as an int
    // check if enough time has passed to reset budget
    if (elapsed.count() >= periodMs) {
        budgetRemaining = budgetLimit;
        lastResetTime = now;
    }
}

void FlowQueue::addDroppedPacket(const Packet& pkt) {
    std::lock_guard<std::mutex> lock(queueMutex);
    droppedPackets.push_back(pkt);
}

void FlowQueue::printDroppedStats() const {
    std::cout << "[DROP STATS] Dropped packets: " << droppedPackets.size() << std::endl;
}


const std::vector<Packet>& FlowQueue::getDroppedPackets() const {
    return droppedPackets;
}

