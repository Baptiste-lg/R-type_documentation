/*
** EPITECH PROJECT, 2024
** Rtype
** File description:
** SYSTEM
*/

/**
 * ClientSystem handles client-server communication.
 *
 * It initializes TCP and UDP sockets to connect to the server.
 * Spawns a thread to listen for incoming UDP messages.
 * Fires events on the registry when messages are received.
 * Sends UDP messages to server when events occur.
 *
 * Usage:
 * - Call tryToJoin() to connect to the server.
 * - Implement event handling for server messages.
 * - Fire events to trigger messages sent to server.
 *
 * Parameters:
 * - registry: Reference to the ECS registry.
 * - deltaTime: Time elapsed since last tick.
 *
 * Components used:
 * - State: Game state component, not directly used.
 *
 * Events:
 * - PlayerJoined: Sent to server on connection.
 * - PlayerMoved: Sent to server on input.
 * - Server event types: Received from server.
*/

#ifndef CLIENTSYSTEM_HPP_
#define CLIENTSYSTEM_HPP_
#include "../include/System/ISystem.hpp"
#include "../include/System/System.hpp"
#include "../include/TextureManager.hpp"
#include "../include/Registry.hpp"
#include "../include/ConfigServer.hpp"
#include "../include/Event.hpp"
#include "../include/Encode.hpp"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <unordered_map>
#include <fcntl.h>
//clear game state
//registry.setPath(gui.menu.getActualSongName());
//initializeGameEntitiesFromConfig(registry, "Scene/GameScene.cfg");




class ClientSystem : public ISystem<State> {
private:
    int tcp_sockfd;
    int udp_sockfd;
    std::thread listener_thread;
    std::thread tcp_listener_thread;  // New attribute

    Registry& registry;
    float& deltaTime;
    struct sockaddr_in serverAddr;
    bool isConnected = false;
    bool isTryingToConnect = false;
    bool askForLobby = false;
    bool askForListLobby = false;
    bool isReady = false;
    std::string requestContent = "";
    int idUdp = -1;
    std::chrono::steady_clock::time_point lastHeartbeatTime;
    const std::chrono::seconds heartbeatInterval = std::chrono::seconds(1);

public:
    std::string const getSystemName() override {
        return "ClientSystem";
    }
    ClientSystem(Registry& registry, float& deltaTime) : registry(registry), deltaTime(deltaTime), lastHeartbeatTime(std::chrono::steady_clock::now()){
        registry.eventBus.addListener(ClientTryConnect, [this](const Event& event) {
            // will try to connect to the server
            if (isConnected) {
                return;
            }
            isTryingToConnect = true;
        });
        registry.eventBus.addListener(EndOfSong, [this,&registry](const Event& event) {
            // will try to connect to the server
            //will notify the server with PEOS event
            //get the score in the with the state.entity
            auto& gameConfigOpt = registry.get_components<GameConfig>()[registry.stateEntity];
            if (!gameConfigOpt.has_value())
                return;
            auto& config = *gameConfigOpt;


            std::string message = "PEOS:" + std::to_string(idUdp) + ":" + std::to_string(config.playerScore);
            send_message(message);
        });

        registry.eventBus.addListener(AskDataCreationLobby, [this](const Event& event) {
            askForLobby = true;
            requestContent = event.content;
        });
        registry.eventBus.addListener(AskDataListLobby, [this](const Event& event) {
            askForListLobby = true;
            requestContent = event.content;
        });
        registry.eventBus.addListener(LobbySelectionCreated, [this](const Event& event) {
            // Your code to handle LobbyCreated event
        });
        registry.eventBus.addListener(PlayerReady, [this, &registry](const Event& event) {
            // Your code to handle LobbyCreated event
        });
        registry.eventBus.addListener(FFTEnd, [this, &registry](const Event& event) {
            isReady = true;
        });
        registry.eventBus.addListener(GameStarted, [this](const Event& event) {
            lastHeartbeatTime = std::chrono::steady_clock::now();
        });
        registry.eventBus.addListener(LobbyJoinRequested, [this, registry](const Event& event) {
            send_message("LJR:" + event.content);
        });
        registry.eventBus.addListener(LobbyJoined, [this, &registry](const Event& event){

        });
        registry.eventBus.addListener(LocalPlayerMoved, [this, &registry](const Event& event){
            //will send the event to the server
            std::string message = "PM:" + event.content;
            send_message(message);
        });
        registry.eventBus.addListener(WavDataAsked, [this, registry](const Event& event) {
            std::string lobbyId = event.content.substr(0, event.content.find(" "));
            std::string songName = event.content.substr(event.content.find(" ") + 1);

            send_message("WDA:" + lobbyId);
        });
        registry.eventBus.addListener(WavDataSent, [this](const Event& event) {
            // Convert the size to a string
            std::string sizeStr = std::to_string(event.TCPDataSize);

            // Pad the size string with leading zeros to make it 10 characters long
            sizeStr.insert(sizeStr.begin(), 10 - sizeStr.length(), '0');
            std::string content = event.content;
            content.insert(content.begin(), 5 - content.length(), '0');
            // Prepare a message header to indicate the type and size of the incoming data
            std::string header = "WDS:" + sizeStr + content + "\0";
            // Send the header first
            if (send(tcp_sockfd, header.c_str(), header.length(), 0) == -1) {
                perror("TCP send header");
                return;
            }

            // Send the actual WAV data
            if (send(tcp_sockfd, event.wavData.data(), event.wavData.size(), 0) == -1) {
                perror("TCP send wav data");
            }
        });
        registry.eventBus.addListener(WavDataRequested, [this, &registry](const Event& event) {
            std::vector<uint8_t> wavData(event.TCPDataSize);
            ssize_t total_bytes_received = 0;
            ssize_t dataSize = event.TCPDataSize;
            ssize_t bytes_received = 0;

            // Assuming tcp_sockfd is the socket connected to the server
            while (total_bytes_received < dataSize) {
                bytes_received = recv(tcp_sockfd, wavData.data() + total_bytes_received,
                                      dataSize - total_bytes_received, 0);
                if (bytes_received <= 0)
                    return;
                total_bytes_received += bytes_received;
            }

            if (total_bytes_received < dataSize)
                return;
            // Store the WAV data to a file
            std::string signature = event.content;  // Assuming you're using content to store some identifier
            std::ofstream wavFile("./" + signature + ".wav", std::ios::out | std::ios::binary);
            if (!wavFile.is_open())
                return;
            wavFile.write(reinterpret_cast<char*>(wavData.data()), wavData.size());
            wavFile.close();
            // we can now send the message to the server that we now want to join the lobby LobbyJoinRequested
            Event newEvent;
            newEvent.type = LobbyJoinRequested;
            newEvent.content = signature;
            registry.eventBus.fireEvent(newEvent);

        });
        registry.eventBus.addListener(PlayerAdded, [this](const Event& event) {
            idUdp = std::stoi(event.content);
            isConnected = true;
        });
        registry.eventBus.addListener(TCPInit, [this](const Event& event){
            std::string message = "UDP:" + event.content;
            send_message(message);
        });
        registry.eventBus.addListener(AskLobbyLeft, [this](const Event& event){
            //the tcp message contains the id of the client in tcp so we will answer on the UDP for the server to be able to link both sockets
            std::string message = "LL:";
            send_message(message);
        });
        registry.eventBus.addListener(AskLobbyDestroyed, [this](const Event& event){
            //the tcp message contains the id of the client in tcp so we will answer on the UDP for the server to be able to link both sockets
            std::string message = "LD:";
            send_message(message);
        });
    }
        int tryToJoin()
    {
        // Initialize TCP
        std::string serverIP = readFromConfig("server_ip");
        int tcpPort = std::stoi(readFromConfig("tcp_port"));

        tcp_sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if (tcp_sockfd == -1) {
            perror("TCP socket");
            return 1;
        }
        int broadcast = 0;

        setsockopt(tcp_sockfd, SOL_SOCKET, SO_BROADCAST, &broadcast, sizeof(broadcast));
        // Error handling

        memset(&serverAddr, 0, sizeof(serverAddr));
        serverAddr.sin_family = AF_INET;
        serverAddr.sin_port = htons(tcpPort);
        serverAddr.sin_addr.s_addr = inet_addr(serverIP.c_str());

        if (connect(tcp_sockfd, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == -1) {
            perror("TCP connect");
            return 1;
        }

        int udpPort = std::stoi(readFromConfig("udp_port"));
        udp_sockfd = socket(AF_INET, SOCK_DGRAM, 0);
        if (udp_sockfd == -1) {
            perror("UDP socket");
            return 1;
        }        // Error handling
        setsockopt(udp_sockfd, SOL_SOCKET, SO_BROADCAST, &broadcast, sizeof(broadcast));


        if (listener_thread.joinable()) {
            listener_thread.join();  // Or listener_thread.detach();
        }
        listener_thread = std::thread(&ClientSystem::listen_to_server, this);

        if (tcp_listener_thread.joinable()) {
            tcp_listener_thread.join();  // Or tcp_listener_thread.detach();
        }
        tcp_listener_thread = std::thread(&ClientSystem::listen_to_server_tcp, this);

        // Fire an initial event to inform the server that a new player has joined.
        // Send "PJ:" message to server
        return 0;
    }

    ~ClientSystem() {
        listener_thread.join();
        tcp_listener_thread.join();
        close(udp_sockfd);
        close(tcp_sockfd);
    }

    void listen_to_server_tcp() {
        char headerBuffer[20];
        while (true) {
            ssize_t bytes_received = recv(tcp_sockfd, headerBuffer, sizeof(headerBuffer) - 1, 0);

            if (bytes_received <= 0)
                return;

            headerBuffer[bytes_received] = '\0';
            std::string headerStr(headerBuffer, bytes_received);
            std::string eventTypeStr = std::string(headerBuffer).substr(0, 3);
            int dataSize = std::stoi(headerStr.substr(4, 10));
            std::string content = headerStr.substr(14, 20);
            EventType eventType = parseEventType(eventTypeStr);  // Assume you have a function that converts string to EventType
            Event newEvent;
            newEvent.type = eventType;
            newEvent.content = content;
            newEvent.TCPDataSize = dataSize;
            registry.eventBus.fireEvent(newEvent);
        }
    }


    void listen_to_server() {
        char buffer[1024];
        while (true) {
            struct sockaddr_in serverAddr;
            socklen_t serverAddrLen = sizeof(serverAddr);
            ssize_t recvLen = recvfrom(udp_sockfd, buffer, 1024, 0, (struct sockaddr*)&serverAddr, &serverAddrLen);
            // Error handling

            buffer[recvLen] = '\0';
            std::string receivedMessage(buffer);
            std::string eventTypeStr = receivedMessage.substr(0, receivedMessage.find(":"));
            std::string eventContent = receivedMessage.substr(receivedMessage.find(":") + 1);
            Event newEvent;
            //might need to add a spliter to the buffer to get the event type
            newEvent.content = eventContent;
            newEvent.type = parseEventType(eventTypeStr);
            registry.eventBus.fireEvent(newEvent);
        }
    }

    void send_message(const std::string& message) {
        // Encode the message to a byte vector
        std::vector<uint8_t> byte_message = encodeToBytes(message);

        struct sockaddr_in serverAddr;
        memset(&serverAddr, 0, sizeof(serverAddr));
        serverAddr.sin_family = AF_INET;
        serverAddr.sin_port = htons(std::stoi(readFromConfig("udp_port")));
        serverAddr.sin_addr.s_addr = inet_addr(readFromConfig("server_ip").c_str());


        // Send the byte vector as a UDP message
        if (sendto(udp_sockfd, byte_message.data(), byte_message.size(), 0,
                (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == -1) {
            perror("UDP sendto");
        }
    }

    void sendMessageTCP(const std::string& message) {
        if (send(tcp_sockfd, message.c_str(), message.length(), 0) == -1) {
            perror("TCP send");
        }
    }
    void operator()(entity_t entity, sparse_array<State>& states) override {
        // Game logic and event triggering
        if (!isConnected) {
            return;
        }
        //check the state if in game part than we will handle the heart beat message here
        auto& state = *states[entity];
        if (state.state == GameState::GAME) {
            auto now = std::chrono::steady_clock::now();
            if (now - lastHeartbeatTime > heartbeatInterval) {
                // It's time to send a heartbeat
                entity_t localPlayer = registry.clientToEntityMap[idUdp];
                auto& positionOpt = registry.get_components<Position>()[localPlayer];
                if (!positionOpt.has_value())
                    return;
                auto& position = *positionOpt;
                //create two float that will be the same as
                //transform float to int and then to string
                int x = position.x;
                int y = position.y;
                std::string message = "HB:" + std::to_string(idUdp) + ":" + std::to_string(x) + ":" + std::to_string(y);
                send_message(message);
                lastHeartbeatTime = now; // Reset timer
            }
            //we will send a message to the server to tell him that we are still alive and the position of the player.
            //we have al the necessary data in the registry client to entity map
        }
    }

    void PostUpdate() override {
        if (isTryingToConnect) {
            isTryingToConnect = false;
            tryToJoin();
        }
        if (askForLobby) {
            askForLobby = false;
            send_message("ADCL:" + requestContent);
            requestContent = "";
        }
        if (askForListLobby) {
            askForListLobby = false;
            send_message("ADLL: " + requestContent);
            requestContent = "";
        }
        if (isReady) {
            isReady = false;
            send_message("PLR:");
        }
    }
};

#endif /* !CLIENTSYSTEM_HPP_ */