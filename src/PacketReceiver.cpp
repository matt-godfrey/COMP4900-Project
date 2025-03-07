/*
 * PacketReceiver.cpp
 *
 *  Created on: Mar. 4, 2025
 *      Author: matt
 */

#include "PacketReceiver.h"
#include <iostream>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cstring> // memset

//PacketReceiver::PacketReceiver() {
//	// AF_NET: specifies IPv4 protocol family
//	// SOCK_RAW: raw network protocol access
//	// IPPROTO_UDP: specifies UDP protocol
//	// alternatively: socket(AF_INET,SOCK_DGRAM,0) should work
//	sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_UDP);
//	if (sockfd < 0 ) {
//		perror("Socket creation failed");
//		exit(EXIT_FAILURE);
//	}
//}


PacketReceiver::PacketReceiver(std::queue<std::string>& q, std::mutex& m, std::condition_variable& cv)
    : packetQueue(q), queueMutex(m), packetAvailable(cv), running(true) {

	// create UDP socket
	// AF_INET: uses IPv4
	// SOCK_DGRAM: uses UDP
	// 0: use default UDP protocol
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // config socket settings
    // tells OS which IP and port socket should use
    struct sockaddr_in addr;
    // clear out struct
//    memset(&addr, 0, sizeof(addr));
    struct sockaddr_in addr = {};
    addr.sin_family = AF_INET;
    // accept packets from any IP; convert to network byte order
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    // set socket to listen on port 9999
    addr.sin_port = htons(9999);

    // bind socket to address
    if (bind(sockfd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }
}

PacketReceiver::~PacketReceiver() {
    running = false;
    close(sockfd);
}

void PacketReceiver::capturePackets() {
	// temp storage for packets
    char buffer[2048];
    // store IP and port of sender
    struct sockaddr_in senderAddr;
    socklen_t senderLen = sizeof(senderAddr);

    while (running) {
    	// receive UDP packet
    	// (struct sockaddr*): cast sockaddr_in to more generic sockaddr for recvfrom
        int len = recvfrom(sockfd, buffer, sizeof(buffer) - 1, 0, (struct sockaddr*)&senderAddr, &senderLen);
        if (len > 0) {
        	// null terminate
            buffer[len] = '\0';
            // copy buffer into packet var
            std::string packet(buffer);

            // lock queue when storing new packets
            std::lock_guard<std::mutex> lock(queueMutex);
            packetQueue.push(packet);
            packetAvailable.notify_one();

            // convert sender IP to readable format
            std::cout << "Received packet from " << inet_ntoa(senderAddr.sin_addr) << ": " << packet << std::endl;
        }
    }
}


//void PacketReceiver::capturePackets() {
////	char buffer[2048];
//	char* buffer = new char[2048];
//
//    while (true) {
//    	// sockfd: socket file descriptor
//    	// buffer: points to buffer where msg should be stored
//    	// len: sizeof(buffer); size of buffer
//    	// flags
//    	int len = recv(sockfd, buffer, sizeof(buffer), 0);
//    	if (len > 0) {
//    		std::cout << "Received packet" << std::endl;
//    		std::cout << "	size: " << len << "bytes" << std::endl;
//    	}
//    	else {
//    		perror("error capturing packet");
//    	}
////        std::cout << "Capturing packet..." << std::endl;
////        sleep(1);
//    }
//
//    delete[] buffer;
//}



