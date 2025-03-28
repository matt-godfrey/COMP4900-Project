#ifndef PACKET_PROCESSOR_H
#define PACKET_PROCESSOR_H

#include <queue>
#include <mutex>
#include <condition_variable>
#include "SharedContext.h"

class PacketProcessor {
private:
//    std::queue<std::string>& packetQueue;
//    std::mutex& queueMutex;
//    std::condition_variable& packetAvailable;
	SharedContext& context;
    bool running;

public:
    PacketProcessor(SharedContext& ctx);
    void processPackets();
};

#endif

