/*
** EPITECH PROJECT, 2023
** Rtype
** File description:
** main.cpp
*/

/**
** main.cpp
**
** Implements a simple UDP server that echoes back received messages.
**
** Methods:
**
** - main()
**   Entry point of the program.
**   Initializes a UDP socket, binds it to a port, and enters a loop
**   where it waits for incoming messages and echoes them back.
**
** Members:
**
** - PORT
**   The port on which the server listens for incoming messages.
**
** - BUFFER_SIZE
**   The size of the buffer used for receiving messages.
**
** - sockfd
**   The socket file descriptor.
**
** - serverAddr
**   The server's address structure.
**
** - clientAddr
**   The client's address structure, filled in by recvfrom.
**
*/

#include <iostream>
#include <cstring>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

int main() {
    const int PORT = 12345;
    const int BUFFER_SIZE = 1024;

    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd == -1) {
        perror("socket");
        return 1;
    }

    struct sockaddr_in serverAddr;
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    if (bind(sockfd, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == -1) {
        perror("bind");
        close(sockfd);
        return 1;
    }

    while (true) {
        char buffer[BUFFER_SIZE];
        struct sockaddr_in clientAddr;
        socklen_t clientAddrLen = sizeof(clientAddr);

        ssize_t recvLen = recvfrom(sockfd, buffer, BUFFER_SIZE, 0, (struct sockaddr*)&clientAddr, &clientAddrLen);
        if (recvLen == -1) {
            perror("recvfrom");
            break;
        }
        buffer[recvLen] = '\0';
    }
    close(sockfd);
    return 0;
}
