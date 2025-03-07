#ifndef PACKET_QUEUE_H
#define PACKET_QUEUE_H

#include <queue>
#include <mutex>

class PacketQueue {
public:
    void enqueue(int packet);
    int dequeue();
    bool isEmpty();

private:
    std::queue<int> queue;
    std::mutex queueMutex;
};

#endif // PACKET_QUEUE_H





//#include <queue>
//#include <mutex>
//
//class PacketQueue {
//public:
//    void enqueue(const std::vector<char>& packet);
//    std::vector<char> dequeue();
//    bool isEmpty();
//
//private:
//    std::queue<std::vector<char>> packetQueue;
//    std::mutex queueMutex;
//    static constexpr int MAX_QUEUE_SIZE = 100;
//};
