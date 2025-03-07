#include "PacketQueue.h"

void PacketQueue::enqueue(int packet) {
    std::lock_guard<std::mutex> lock(queueMutex);
    queue.push(packet);
}

int PacketQueue::dequeue() {
    std::lock_guard<std::mutex> lock(queueMutex);
    if (queue.empty()) return -1;
    int packet = queue.front();
    queue.pop();
    return packet;
}

bool PacketQueue::isEmpty() {
    std::lock_guard<std::mutex> lock(queueMutex);
    return queue.empty();
}
