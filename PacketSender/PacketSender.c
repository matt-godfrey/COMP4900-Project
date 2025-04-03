#include <stdio.h> 
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <arpa/inet.h>



//#define TARGET_IP "192.168.0.176"
//#define TARGET_PORT 9999
#define MAX_PACKETS 5
#define BURST_COUNT 10

const char* test_packets[] = {
    "SRC=192.168.0.101|PROTO=UDP|PORT=9999|DATA=EMERGENCY_STOP",
    "SRC=192.168.0.102|PROTO=UDP|PORT=8888|DATA=TEMP_UPDATE",
    "SRC=192.168.0.103|PROTO=ICMP|PORT=0|DATA=PING",
    "SRC=192.168.0.104|PROTO=TCP|PORT=22|DATA=SSH_REQUEST",
    "SRC=192.168.0.105|PROTO=UDP|PORT=7777|DATA=STATUS"
};

void send_packet(int sockfd, struct sockaddr_in* dest_addr, const char* msg) {
    sendto(sockfd, msg, strlen(msg), 0, (struct sockaddr*)dest_addr, sizeof(*dest_addr));
    printf("Sent: %s\n", msg);
}



int main(int argc, char* argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <target_ip> [port]\n", argv[0]);
        return 1;
    }

    const char* target_ip = argv[1];
    int target_port = 9999;
    if (argc >= 3) {
        target_port = atoi(argv[2]);
        if (target_port <= 0 || target_port > 65535) {
            fprintf(stderr, "Invalid port number. Must be between 1 and 65535.\n");
            return 1;
        }
    }

    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("Socket creation failed");
        return 1;
    }

    struct sockaddr_in dest_addr;
    memset(&dest_addr, 0, sizeof(dest_addr));
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_port = htons(target_port);
    inet_pton(AF_INET, target_ip, &dest_addr.sin_addr);

    srand(time(NULL)); 

    while (1) {
        printf("\n--- Packet Sender Menu ---\n");
        printf("1. Send all test packets in sequence (1 every 3 sec)\n");
        printf("2. Send one random packet\n");
        printf("3. Send burst of random packets\n");
        printf("4. Send burst of only ICMP packets\n");
        printf("5. Send burst of only UDP packets\n");
        printf("6. Exit\n");
        printf("Choose an option: ");

        int option;
        if (scanf("%d", &option) != 1) {
            printf("Invalid input. Try again.\n");
            while (getchar() != '\n');
            continue;
        }

        switch (option) {
            case 1:
                for (int i = 0; i < MAX_PACKETS; i++) {
                    send_packet(sockfd, &dest_addr, test_packets[i]);
                    sleep(3);
                }
                break;
            case 2: {
                int index = rand() % MAX_PACKETS;
                send_packet(sockfd, &dest_addr, test_packets[index]);
                break;
            }
            case 3:
                for (int i = 0; i < BURST_COUNT; i++) {
                    int index = rand() % MAX_PACKETS;
                    send_packet(sockfd, &dest_addr, test_packets[index]);
                    usleep(100 * 1000); // 100 ms delay
                }
                break;
            case 4:
                for (int i = 0; i < BURST_COUNT; i++) {
                    send_packet(sockfd, &dest_addr, "SRC=192.168.0.103|PROTO=ICMP|PORT=0|DATA=PING");
                    usleep(100 * 1000);
                }
                break;
            case 5:
                for (int i = 0; i < BURST_COUNT; i++) {
                    send_packet(sockfd, &dest_addr, "SRC=192.168.0.101|PROTO=UDP|PORT=400|DATA=EMERGENCY_STOP");
                    usleep(100 * 1000);
                }
                break;
            case 6:
                close(sockfd);
                return 0;
            default:
                printf("Invalid option. Try again.\n");
        }
    }

    close(sockfd);
    return 0;
}