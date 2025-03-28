/*
 * SharedContext.h
 *
 *  Created on: Mar. 26, 2025
 *      Author: matt
 */

#ifndef INCLUDE_SHAREDCONTEXT_H_
#define INCLUDE_SHAREDCONTEXT_H_

struct SharedContext {
    std::queue<std::string> highPriorityQ;
    std::queue<std::string> mediumPriorityQ;
    std::queue<std::string> lowPriorityQ;

    std::mutex queueMutex;
    std::condition_variable packetAvailable;
};



#endif /* INCLUDE_SHAREDCONTEXT_H_ */
