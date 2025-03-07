#ifndef RATE_LIMITER_H
#define RATE_LIMITER_H

class RateLimiter {
public:
    void enforceLimit();
};

#endif // RATE_LIMITER_H



//#include <ctime>
//#include <iostream>
//
//class RateLimiter {
//public:
//    RateLimiter();
//    bool enforceRateLimiting();
//
//private:
//    static constexpr int MAX_PACKETS_PER_PERIOD = 50;
//    int processedPackets;
//    std::time_t lastResetTime;
//};
