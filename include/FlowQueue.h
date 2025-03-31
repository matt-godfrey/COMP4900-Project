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
    Packet dequeue();
    bool isEmpty();
    void setPriority(int p);
    int getPriority() const;

    bool canProcess();  // Rate limit check
    void resetBudgetIfNeeded();

private:
    std::vector<Packet> queue;
    std::mutex queueMutex;
    int flowPriority;


    int periodMs;
    int budgetLimit;
    int budgetRemaining;

    std::chrono::steady_clock::time_point lastResetTime;
};

#endif // PACKET_QUEUE_H
