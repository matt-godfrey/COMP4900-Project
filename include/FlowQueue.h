#ifndef FLOW_QUEUE_H
#define FLOW_QUEUE_H

//#include <queue>
#include <vector>
#include <mutex>
#include "Packet.h"

class FlowQueue {
public:
    FlowQueue(int flowPriority = 0, int periodMs = 1000, int budgetLimit = 5);

    void enqueue(const Packet& pkt);
    void enqueueDropped(const Packet& pkt);
    Packet dequeue();
    Packet peek() const;

    bool isEmpty();

    void setPriority(int p);
    int getPriority() const;
    const std::vector<Packet>& getDroppedPackets() const;

    void addDroppedPacket(const Packet& pkt);
    void printDroppedStats() const;


    bool canProcess();  // Rate limit check
    void resetBudgetIfNeeded();


private:
    std::vector<Packet> queue;
    std::mutex queueMutex;
    int flowPriority;

    std::vector<Packet> droppedPackets;

    int periodMs;
    int budgetLimit;
    int budgetRemaining;

    std::chrono::steady_clock::time_point lastResetTime;
};

#endif // PACKET_QUEUE_H
