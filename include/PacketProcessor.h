#ifndef PACKET_PROCESSOR_H
#define PACKET_PROCESSOR_H

#include <queue>
#include <mutex>
#include <condition_variable>

class PacketProcessor {
private:
    std::queue<std::string>& packetQueue;
    std::mutex& queueMutex;
    std::condition_variable& packetAvailable;
    bool running;

public:
    PacketProcessor(std::queue<std::string>& q, std::mutex& m, std::condition_variable& cv);
    void processPackets();
};

#endif

