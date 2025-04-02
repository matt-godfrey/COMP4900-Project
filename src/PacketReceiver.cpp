/*
 * PacketReceiver.cpp
 *
 *  Created on: Mar. 4, 2025
 *      Author: matt
 */

#include "PacketReceiver.h"
#include <iostream>
#include <sstream>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cstring> // memset
#include <iomanip>
#include <ctime>


PacketReceiver::PacketReceiver(SharedContext& ctx)
    : context(ctx), running(true) {


	//std::cout << "UID: " << getuid() << std::endl;

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
//    struct sockaddr_in addr;
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
//        int len = recvfrom(sockfd, buffer, sizeof(buffer) - 1, 0, nullptr, nullptr);
        if (len > 0) {
            buffer[len] = '\0';
            std::string raw(buffer);

            std::string proto, ip, data;
            int port = 0;

            // parse eaach field in packet
            std::istringstream stream(raw);
            std::string token;
            // getline extracts chars from packet stream, appends to token until '|' found
            while (std::getline(stream, token, '|')) {
            	// check if current string val of token is 'SRC'
                if (token.rfind("SRC=", 0) == 0)
                	// if so, copy chars starting at pos 4 to the end into 'ip'
                    ip = token.substr(4);
                else if (token.rfind("PROTO=", 0) == 0)
                    proto = token.substr(6);
                else if (token.rfind("PORT=", 0) == 0)
                	// convert string to int
                    port = std::stoi(token.substr(5));
                else if (token.rfind("DATA=", 0) == 0)
                    data = token.substr(5);
            }

            // priority based on protocol/port
            int priority = assignPriority(proto, port);


            Packet pkt(ip, proto, port, data, priority);
            pkt.timestamp = std::chrono::system_clock::now();

            std::lock_guard<std::mutex> lock(context.contextMutex);
            if (proto == "ICMP" || proto == "ARP")
                context.icmp_arpQueue.enqueue(pkt);
            else if (proto == "UDP" && port == 400)
                context.udpQueue_400.enqueue(pkt);
            else if (proto == "UDP")
                context.udpQueue.enqueue(pkt);
            else if (proto == "TCP" && port == 80)
                context.tcpQueue_80.enqueue(pkt);
            else if (proto == "TCP" && port == 443)
                context.tcpQueue_443.enqueue(pkt);
            else
                context.defaultQueue.enqueue(pkt);  // unknown traffic
//                std::cout << "Unrecognized packet format: " << raw << std::endl;

            // let PacketProcessor know packet has arrived
            context.packetAvailable.notify_one();

//            std::time_t timeT = std::chrono::system_clock::to_time_t(pkt.timestamp);
//
//            std::cout << "-------------------------------------------------------------------------------------------------------------" << std::endl;
//            // convert sender IP to readable format
//            std::cout << "["
//            		<< std::put_time(std::localtime(&timeT), "%Y-%m-%d %H:%M:%S") << "] "
//					<< "Received packet from " << inet_ntoa(senderAddr.sin_addr) << ": " << raw << std::endl;
        }
    }
}

int PacketReceiver::assignPriority(const std::string& proto, int port) {
    if (proto == "ICMP" || proto == "ARP") return 3;
    if (proto == "UDP" && port == 400) return 2;
    if (proto == "TCP" && (port == 80 || port == 443)) return 1;
    return 0;
}



