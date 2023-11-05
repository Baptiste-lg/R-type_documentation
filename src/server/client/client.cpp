/*
** EPITECH PROJECT, 2023
** Rtype
** File description:
** client.cpp
*/

/**
* client.cpp
*
* Implements a simple UDP client to send messages to a server.
*
* Creates a UDP socket and connects to a server IP/port.
* Sends multiple messages in a loop to the server.
*
* Methods:
*
* - main()
*   - Creates UDP socket.
*   - Connects to server IP/port.
*   - Loops sending messages to server.
*
* Constants:
*
* - SERVER_IP
*   IP address of server.
*
* - SERVER_PORT
*   Port number of server.
*
* - BUFFER_SIZE
*   Size of network buffer.
*/

#include <iostream>
#include <cstring>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

int main()
{
    const char *SERVER_IP = "127.0.0.1";
    const int SERVER_PORT = 12345;
    const int BUFFER_SIZE = 1024;

    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd == -1)
    {
        perror("socket");
        return 1;
    }

    struct sockaddr_in serverAddr;
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(SERVER_PORT);
    serverAddr.sin_addr.s_addr = inet_addr(SERVER_IP);

    // while (true)
    // {
        std::string message;
        // std::getline(std::cin, message);
        int i = 0;

        while (i <= 100)
        {
            std::string string = "Message is ";
            string += std::to_string(i);
            ssize_t sendLen = sendto(sockfd, string.c_str(), string.length(), 0, (struct sockaddr *)&serverAddr, sizeof(serverAddr));
            if (sendLen == -1) {
                perror("sendto");
                break;
            }
            i++;
        }
        // if (message == "q")
        //     break;
    // }

    close(sockfd);

    return 0;
}
