/*
** EPITECH PROJECT, 2024
** Rtype
** File description:
** ServerSystem.hpp
*/

/**
 * ServerSystem handles server-side networking and state.
 *
 * It initializes TCP and UDP sockets to listen for clients.
 * Accepts TCP connections from clients.
 * Receives UDP messages and fires corresponding events.
 * Maintains state like connected clients, entities, and lobbies.
 * Sends UDP messages to clients when events occur.
 *
 * Usage:
 * - Call constructor to initialize sockets.
 * - Implement event handling for client messages.
 * - Maintain server state as needed.
 * - Fire events to trigger messages sent to clients.
 *
 * Parameters:
 * - registry: Reference to the ECS registry.
 * - deltaTime: Time elapsed since last tick.
 *
 * Components used:
 * - Network: Network component, not directly used.
 *
 * Events:
 * - PlayerJoined: Received when a client connects.
 * - PlayerMoved: Received on client input.
 * - Server event types: Sent to clients.
*/

#ifndef SERVERSYSTEM_HPP_
#define SERVERSYSTEM_HPP_

#include "../include/System/ISystem.hpp"
#include "../include/System/System.hpp"
#include "../include/TextureManager.hpp"
#include "../include/Registry.hpp"
#include "../include/Event.hpp"
#include "../include/Global.hpp"
#include "../include/Encode.hpp"
#include "../ConfigServer.hpp"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include <unordered_map>
#include <fcntl.h>
#include <cstdint>


struct lobby {
    std::string name;
    int id;
    std::string artistName;
    std::string songName;
    bool musicStored = false;
    std::vector<std::tuple<int, entity_t, bool >> players; //id, entity id, ready
    bool isGameStarted = false;
    std::map<int, int> scoreMap; //id, score
};


class ServerSystem : public ISystem<Network> {
private:
    float &deltaTime;
    int clientIdCounter = 0;
    int tcp_sockfd;
    int udp_sockfd;
    Registry &registry;
    std::unordered_map<EventType, std::function<void(
            const Event &)>> eventHandlerMap;
    std::vector<std::vector<int>> clientToEntityMap;
    std::vector<lobby> activeLobbies; // To hold active lobbies
    int nextLobbyId = 1; // To generate unique lobby IDs
    std::queue<int> newPlayersQueue;
    std::queue<std::tuple<std::string, int, int>> newLobbiesQueue;
    std::queue<int> sendLobbyListQueue;
    std::queue<int> startGameQueue;
    std::queue<int> TCPsocketQueue;
    std::map<int, std::tuple<struct sockaddr_in, int>> clientInfoMap; // Maps clientID to a tuple of (UDP id, UDP address, TCP socket) and unique id as key

// Replace vector with map for clients
    //content, id of client, id of lobby
    std::tuple<std::string, int, int> contentIDLobbyCreation;
public:

    int getTCPsocketWithUniqueId(int id) {
        //check the key of the map
        for (auto &client: clientInfoMap) {
            if (client.first == id) {
                return std::get<1>(client.second);
            }
        }
        return -1;
    }

    sockaddr_in &getUDPAdrWithTCP(int tcpClientID) {
        for (auto &client: clientInfoMap) {
            if (std::get<1>(client.second) == tcpClientID) {
                return std::get<0>(client.second);
            }
        }
        throw std::runtime_error("No match found");

    }

    sockaddr_in &getUDPAdrWithUniqueId(int id) {
        for (auto &client: clientInfoMap) {
            if (client.first == id) {
                return std::get<0>(client.second);
            }
        }
        throw std::runtime_error("No match found");

    }

    int getUniqueIdWithTCP(int tcpClientID) {
        for (auto &client: clientInfoMap) {
            if (std::get<1>(client.second) == tcpClientID) {
                return client.first;
            }
        }
        return -1;
    }

    int getUniqueIdWithUDP(int udpClientID) {
        for (auto &client: clientInfoMap) {
            if (std::get<0>(client.second).sin_port == udpClientID) {
                return client.first;
            }
        }
        return -1;
    }

    void addListerner(Registry &registry) {
        registry.eventBus.addListener(UDPInit, [this, &registry](const Event &event) {
            //this is a message that will be received trought udp containing the client id of the TCP, we will link it to a unique id and the udp address
            //we will also triger newPlayersQueue.push(event.clientID); to add a client entity to the game
            int tcpClientID = std::stoi(event.content);
            // check in the queue if the client id is present
            //if its present we will add it to the map
            struct sockaddr_in udp_clientAddr = event.UDPAddr;
            int newIndex = 0;
            while (!TCPsocketQueue.empty()) {
                int client = TCPsocketQueue.front(); // Get the front element
                TCPsocketQueue.pop(); // Remove the front element
                if (client == tcpClientID) {
                    //get the udp address with the client id
                    clientInfoMap[clientIdCounter] = std::make_tuple(udp_clientAddr, tcpClientID);
                    newIndex = clientIdCounter;
                    break;
                }
                TCPsocketQueue.push(client);
            }
            newPlayersQueue.push(newIndex);
            //send PlayerAdded with the udp to the client that just connected
            std::string message = "PA:" + std::to_string(clientIdCounter);
            clientIdCounter++;

            sendto(udp_sockfd, message.c_str(), message.length(), 0,
                   (struct sockaddr *) &udp_clientAddr,
                   sizeof(udp_clientAddr));
        });
        registry.eventBus.addListener(PlayerJoined, [this, &registry](const Event &event) {
            newPlayersQueue.push(event.clientID);
        });
        registry.eventBus.addListener(PlayerLeft, [this, &registry](
                const Event &event) {
            //we will remove the entity from the game
        });
        registry.eventBus.addListener(PlayerMoved, [this, &registry](
                const Event &event) {

            //will receive : PM:key
            //will send : PM:key:id to all the other client in his lobby if its active
            //check if inside a lobby
            auto it = std::find_if(activeLobbies.begin(), activeLobbies.end(), [&event](const auto &lobby) {
                return std::any_of(lobby.players.begin(), lobby.players.end(), [&event](const auto &player) {
                    return std::get<0>(player) == event.clientID;
                });
            });
            if (it != activeLobbies.end()  && it->isGameStarted) {
                // Found the lobby, now inform all players in this lobby
                std::string message = "PM:" + std::to_string(event.clientID) + ":" + event.content;
                for (const auto &player : it->players) {
                    if (std::get<0>(player) != event.clientID) {
                        sendto(udp_sockfd, message.c_str(), message.length(), 0, (struct sockaddr *) &getUDPAdrWithUniqueId(std::get<0>(player)), sizeof(getUDPAdrWithUniqueId(std::get<0>(player))));
                    }
                }
            }
        });
        registry.eventBus.addListener(AskDataCreationLobby, [this, &registry](const Event &event) {
            newLobbiesQueue.push(std::make_tuple(event.content, event.clientID, nextLobbyId++));
        });
        registry.eventBus.addListener(LobbyJoinRequested, [this, &registry](
                const Event &event) {
            // Handle joining a lobby
            int lobbyId = std::stoi(event.content);
            // Add the player to the corresponding lobby
            // ... (lookup lobby and add player)
            int indexLookup = 0;
            bool found = false;
            for (auto &lobby: activeLobbies) {
                indexLookup++;
                if (lobby.id == lobbyId) {
                    lobby.players.push_back({event.clientID,
                                             registry.clientToEntityMap[event.clientID], false});


                    int nbPlayer = 0;
                    for (auto &player: lobby.players)
                        nbPlayer++;
                    // Send a LobbyJoined event
                    std::string message = "LJ:" + std::to_string(lobbyId) + ":" + std::to_string(event.clientID) + ":" + lobby.name + ":" + lobby.artistName + ":" + lobby.songName + ":" + std::to_string(nbPlayer);
                    //add the id and if ready or not of the joining player
                    for (auto &player: lobby.players) {
                        message += ":" + std::to_string(std::get<0>(player)) + ":" + std::to_string(std::get<2>(player));
                    }
                    message += ":";
                    sendto(udp_sockfd, message.c_str(), message.length(), 0,
                           (struct sockaddr *) &event.UDPAddr,
                           sizeof(event.UDPAddr));
                    //will send lobby id,cliend
                    message = "PJL:" + std::to_string(event.clientID) + ":" + std::to_string(lobbyId);
                    for (auto &player: lobby.players) {
                        if (std::get<0>(player) != event.clientID) {
                            sendto(udp_sockfd, message.c_str(), message.length(), 0,
                                   (struct sockaddr *) &getUDPAdrWithUniqueId(std::get<0>(player)),
                                   sizeof(getUDPAdrWithUniqueId(std::get<0>(player))));
                        }
                    }
                    return;
                }
            }
        });

        registry.eventBus.addListener(AskDataListLobby, [this, &registry](
                const Event &event) {
            // Handle get lobby list request
            sendLobbyListQueue.push(event.clientID);
        });
        registry.eventBus.addListener(HeartBeat, [this, &registry](
                const Event &event) {
            // Handle heartbeat
            // this will be send by the client during the game at regular interval
            // we will send it back to the other client in the game to let the client correct the position of the entity
            // the message received is HB:clientId:posX:posY
            // we will send it back to the other client in the game with the same message
            int clientId = std::stoi(event.content.substr(0, event.content.find(":")));
            std::string message = "HB:" + event.content;
            auto it = std::find_if(activeLobbies.begin(), activeLobbies.end(),
                                   [&event](const auto &lobby) {
                //check the lobby where game is started
                if (lobby.isGameStarted) {
                    return std::any_of(lobby.players.begin(), lobby.players.end(),
                                       [&event](const auto &player) {
                                           return std::get<0>(player) == std::stoi(event.content.substr(0, event.content.find(":")));
                                       });
                }
                return false;
            });
            if (it != activeLobbies.end()) {
                // Found the lobby, now inform all players in this lobby
                for (const auto &player : it->players) {
                    if (std::get<0>(player) != event.clientID) {
                        sendto(udp_sockfd, message.c_str(), message.length(), 0,
                               (struct sockaddr *) &getUDPAdrWithUniqueId(std::get<0>(player)),
                               sizeof(getUDPAdrWithUniqueId(std::get<0>(player))));
                    }
                }
            }


        });
        registry.eventBus.addListener(LobbyMessageSent, [this, &registry](
                const Event &event) {
            // Handle lobby message
            // Send message to all players in the lobby
            int lobbyId = std::stoi(event.content);
            std::string message = "LMS:" + std::to_string(lobbyId) + ":" + std::to_string(event.clientID) + ":" +
                                  event.content;
            for (auto &lobby: activeLobbies) {
                if (lobby.id == lobbyId) {
                    for (auto &player: lobby.players) {
                        sendto(udp_sockfd, message.c_str(), message.length(), 0,
                               (struct sockaddr *) &getUDPAdrWithUniqueId(std::get<0>(player)),
                               sizeof(getUDPAdrWithUniqueId(std::get<0>(player))));
                    }
                }
            }
        });
        registry.eventBus.addListener(LobbyDestroyed, [this, &registry](const Event &event) {

            auto itLobby = std::find_if(activeLobbies.begin(), activeLobbies.end(),
                                        [&event](const auto &lobby) {
                                            return std::any_of(lobby.players.begin(), lobby.players.end(),
                                                               [&event](const auto &player) {
                                                                   return std::get<0>(player) == event.clientID;
                                                               });
                                        });

            if (itLobby != activeLobbies.end()) {
                // Found the lobby, now inform all players in this lobby
                std::string message = "LD:" + std::to_string(itLobby->id);
                for (const auto &player : itLobby->players) {
                    sendto(udp_sockfd, message.c_str(), message.length(), 0,
                           (struct sockaddr *) &getUDPAdrWithUniqueId(std::get<0>(player)),
                           sizeof(getUDPAdrWithUniqueId(std::get<0>(player))));
                }
                // Remove the lobby
                activeLobbies.erase(itLobby);
                //will remove the music from the serverDB its named after the lobby id + 0 until 5 char + wav
                //we will also reuse the id of the lobby
                std::string lobbymusic= std::to_string(itLobby->id);
                lobbymusic.insert(lobbymusic.end(), 5 - lobbymusic.size(), '0');
                std::string path = "asset/Music/" + lobbymusic + ".wav";
                std::remove(path.c_str());
            }
        });

        registry.eventBus.addListener(LobbyLeft, [this, &registry](const Event &event) {
            for (auto &lobby: activeLobbies) {
                auto it = std::remove_if(lobby.players.begin(), lobby.players.end(),
                                         [&event, this, &lobby](const auto &player) {
                                             if (std::get<0>(player) == event.clientID) {
                                                 // Send message to inform that the player has left the lobby
                                                 std::string message = "LL:" + std::to_string(lobby.id) + ":" + std::to_string(event.clientID);
                                                 sendto(udp_sockfd, message.c_str(), message.length(), 0,
                                                        (struct sockaddr *) &getUDPAdrWithUniqueId(std::get<0>(player)),
                                                        sizeof(getUDPAdrWithUniqueId(std::get<0>(player))));
                                                 return true; // This will remove the player
                                             }
                                             // Send message to other players in the lobby to inform that a player has left
                                             std::string message = "PLL:" + std::to_string(lobby.id) + ":" + std::to_string(event.clientID);
                                             sendto(udp_sockfd, message.c_str(), message.length(), 0,
                                                    (struct sockaddr *) &getUDPAdrWithUniqueId(std::get<0>(player)),
                                                    sizeof(getUDPAdrWithUniqueId(std::get<0>(player))));
                                             return false; // This will keep the player
                                         });
                if (it != lobby.players.end()) {
                    lobby.players.erase(it, lobby.players.end()); // Actual removal from the vector
                    return;
                }
            }
        });

        registry.eventBus.addListener(PlayerReady, [this, &registry](const Event &event) {

            // Get client ID from event
            int clientId = event.clientID;

            // Find which lobby this client is in
            int lobbyIdc = -1;
            int lobbyId = 0;
            for (auto &lobby: activeLobbies) {
                lobbyIdc++;
                for (auto &player: lobby.players) {
                    if (std::get<0>(player) == clientId) {
                        lobbyId = lobby.id;
                        //set the player to ready
                        std::get<2>(player) = true;
                        break;
                    }
                }
                if (lobbyId != -1) {
                    break;
                }
            }

            if (lobbyId == -1) {
                // Client not in a lobby, error
                return;
            }
            // Check if all ready
            size_t readyCount = 0;
            //use find to get the lobby that containt a field .id == lobbyId
            for (auto &lobby: activeLobbies) {
                if (lobby.id == lobbyId) {
                    for (auto &player: lobby.players) {
                        if (std::get<2>(player)) {
                            readyCount++;
                        }
                    }
                    break;
                }
            }
            //send a PR:lobbyId:clientId to all the players in the lobby
            std::string message = "LPR:" + std::to_string(clientId);
            for (auto &lobby: activeLobbies) {
                if (lobby.id == lobbyId) {
                    for (auto &player: lobby.players) {
                        sendto(udp_sockfd, message.c_str(), message.length(), 0,
                               (struct sockaddr *) &getUDPAdrWithUniqueId(std::get<0>(player)),
                               sizeof(getUDPAdrWithUniqueId(std::get<0>(player))));
                    }
                }
            }
            if (readyCount == activeLobbies[lobbyIdc].players.size()) {
                // All ready, start game
                Event startEvent;
                startEvent.type = GameStarted;
                startEvent.content = std::to_string(lobbyId);
                startEvent.clientID = -1;
                registry.eventBus.fireEvent(startEvent);
            }

        });
        registry.eventBus.addListener(GameStarted, [this, &registry](const Event &event) {
            // Handle game start event
            //we need to check all the client in the lobby and we will add them to the queue
            std::string message;
            //we will create a time stamp and send it to all the client in the lobby, it will be time of right now + 5 seconds
            std::time_t result = std::time(nullptr);
            result += 5;
            for (auto &lobby: activeLobbies) {
                if (lobby.id == std::stoi(event.content)) {
                    lobby.isGameStarted = true;
                    for (auto &player: lobby.players) {
                        message = "GS:" + std::to_string(std::get<0>(player)) + "|" + std::to_string(result);
                        sendto(udp_sockfd, message.c_str(), message.length(), 0,
                               (struct sockaddr *) &getUDPAdrWithUniqueId(std::get<0>(player)),
                                 sizeof(getUDPAdrWithUniqueId(std::get<0>(player))));
                    }
                }
            }
        });
        registry.eventBus.addListener(playerEndOfSong, [this,  &registry](const Event &event) {
            //will check in the lobby and store the score of the player
            std::vector<std::string> eventContent = tokenize(event.content, ":");
            //0 = id of the player, 1 = score
            int idPlayer = std::stoi(eventContent[0]);
            int score = std::stoi(eventContent[1]);
            auto it = std::find_if(activeLobbies.begin(), activeLobbies.end(),
                                   [&idPlayer](const auto &lobby) {
                                       return std::any_of(lobby.players.begin(), lobby.players.end(),
                                                          [&idPlayer](const auto &player) {
                                                              return std::get<0>(player) == idPlayer;
                                                          });
                                   });
            if (it != activeLobbies.end()) {
                // Found the lobby, now inform all players in this lobby
                it->scoreMap[idPlayer] = score;
                int allPlayerFinished = 0;
                //print the full score map
                for (auto &player: it->players) {
                    if (it->scoreMap.count(std::get<0>(player)) > 0){
                        allPlayerFinished++;
                    }
                }
                //add a check to see if all the player have finished the song
                if (allPlayerFinished == it->players.size()){
                    std::string message = "PES:";
                    for (auto &player: it->players) {
                        message += std::to_string(std::get<0>(player)) + ":" + std::to_string(it->scoreMap[std::get<0>(player)]) + ":";
                    }
                    for (auto &player: it->players) {
                        sendto(udp_sockfd, message.c_str(), message.length(), 0,
                               (struct sockaddr *) &getUDPAdrWithUniqueId(std::get<0>(player)),
                               sizeof(getUDPAdrWithUniqueId(std::get<0>(player))));
                    }
                    activeLobbies.erase(it);
                }
            }
        });
        registry.eventBus.addListener(WavDataAsked, [this, &registry](const Event &event) {
            //we will receive WDR + lobby id
            // we will parse ServerDB/lobbyId.wav and send it to the client trought tcp
            std::string lobbyId = event.content;
            //fill lobbyId with 0
            lobbyId.insert(lobbyId.begin(), 5 - lobbyId.length(), '0');
            std::ifstream wavFile("ServerDB/" + lobbyId + ".wav", std::ios::in | std::ios::binary);
            if (!wavFile.is_open()) {
                return;
            }
            // Step 1: Get file size
            std::vector<char> buffer(std::istreambuf_iterator<char>(wavFile), {});
            std::vector<uint8_t> wavData;
            wavData.insert(wavData.end(), buffer.begin(), buffer.end());
            wavFile.close();
            // Step 2: Send header
            std::string sizeStr = std::to_string(wavData.size());
            sizeStr.insert(sizeStr.begin(), 10 - sizeStr.length(), '0');
            std::string header = "WDR:" + sizeStr + lobbyId;
//will now send trought tcp the header and the wavData
            int tcpClientID = getTCPsocketWithUniqueId(event.clientID);
            ssize_t sentBytes = send(tcpClientID, header.c_str(), header.length(), 0);
            if (sentBytes == -1)
                return;
// Use wavData, not event.wavData
            ssize_t total_bytes_sent = send(tcpClientID, wavData.data(), wavData.size(), 0);
            if (total_bytes_sent == -1)
                return;
        });
        registry.eventBus.addListener(WavDataSent, [this, &registry](const Event &event) {
            ssize_t dataSize = event.TCPDataSize;
            std::vector<uint8_t> wavData(dataSize);
            ssize_t total_bytes_received = 0;
            ssize_t bytes_received = 0;
            while (total_bytes_received < dataSize) {
                bytes_received = recv(event.clientID, wavData.data() + total_bytes_received,
                                      dataSize - total_bytes_received, 0);
                if (bytes_received <= 0) {
                    close(event.clientID);
                    return;
                }
                total_bytes_received += bytes_received;
            }
            if (total_bytes_received < dataSize) {

                close(event.clientID);
                return;
            }
            // Step 4: store the content, we used the content to store the lobby id
            std::string signature = event.content;
            std::ofstream wavFile("ServerDB/" + signature + ".wav", std::ios::out | std::ios::binary);
            if (!wavFile.is_open())
                return;
            wavFile.write(reinterpret_cast<char *>(wavData.data()), wavData.size());
            wavFile.close();
            int clientId = getUniqueIdWithTCP(event.clientID);
            std::string message = "PJL:" + std::to_string(clientId) + ":" + std::to_string(std::stoi(event.content));
            //set the lobby     bool musicStored = true;
            //chjeck in all the lobby witch one have a player with id = clientId
            auto it = std::find_if(activeLobbies.begin(), activeLobbies.end(),
                                   [&clientId](const auto &lobby) {
                                       return std::any_of(lobby.players.begin(), lobby.players.end(),
                                                          [&clientId](const auto &player) {
                                                              return std::get<0>(player) == clientId;
                                                          });
                                   });
            if (it != activeLobbies.end())
                it->musicStored = true;
            sendto(udp_sockfd, message.c_str(), message.length(), 0,
                   (struct sockaddr *) &getUDPAdrWithTCP(event.clientID),
                   sizeof(getUDPAdrWithTCP(event.clientID)));
        });
    }

    ServerSystem(Registry &registry, float &deltaTime) : registry(
            registry), deltaTime(deltaTime) {
        tcp_sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if (tcp_sockfd == -1) {
            perror("socket");
            // handle error
            return;
        }
        std::string serverIP = readFromConfig("server_ip");
        // int tcpPort, udpPort;
        // try {
        int tcpPort = std::stoi(readFromConfig("tcp_port"));
        int udpPort = std::stoi(readFromConfig("udp_port"));
            // Use tcpPort and udpPort as needed
        // } catch (const std::invalid_argument& e) {
        //     std::cerr << "Invalid argument exception: " << e.what() << std::endl;
        //     // Handle the exception (e.g., log an error message)
        // } catch (const std::out_of_range& e) {
        //     std::cerr << "Out of range exception: " << e.what() << std::endl;
        //     // Handle the exception (e.g., log an error message)
        // }

        struct sockaddr_in serverAddr;
        memset(&serverAddr, 0, sizeof(serverAddr));
        serverAddr.sin_family = AF_INET;
        serverAddr.sin_port = htons(tcpPort);
        serverAddr.sin_addr.s_addr = inet_addr(serverIP.c_str());

        if (bind(tcp_sockfd, (struct sockaddr *) &serverAddr,
                 sizeof(serverAddr)) == -1) {
            perror("bind");
            // handle error
            return;
        }

        if (listen(tcp_sockfd, 5) == -1) {
            perror("listen");
            // handle error
            return;
        }

        int flags = fcntl(tcp_sockfd, F_GETFL, 0);
        fcntl(tcp_sockfd, F_SETFL, flags | O_NONBLOCK);

        udp_sockfd = socket(AF_INET, SOCK_DGRAM, 0);
        if (udp_sockfd == -1) {
            perror("UDP socket");
            // handle error
            return;
        }

        struct sockaddr_in udp_serverAddr;
        memset(&udp_serverAddr, 0, sizeof(udp_serverAddr));
        udp_serverAddr.sin_family = AF_INET;
        udp_serverAddr.sin_port = htons(udpPort);
        serverAddr.sin_addr.s_addr = inet_addr(readFromConfig("server_ip").c_str());

        if (bind(udp_sockfd, (struct sockaddr *) &udp_serverAddr,
                 sizeof(udp_serverAddr)) == -1) {
            perror("UDP bind");
            // handle error
            return;
        }

        int udp_flags = fcntl(udp_sockfd, F_GETFL, 0);
        fcntl(udp_sockfd, F_SETFL, udp_flags | O_NONBLOCK);
        //will clean the ServerDB folder
        //do it a way usable for all os
        //rm rf is not working on windows
        std::filesystem::path serverDBPath = "ServerDB";

        // Recursively remove all files and folders
        for (const auto &entry: std::filesystem::recursive_directory_iterator(serverDBPath)) {
            std::filesystem::remove_all(entry.path());
        }
        addListerner(registry);
    }


    void operator()(entity_t entity,
                    sparse_array<Network> &networks) override {

        // Listen for new TCP connections
        struct sockaddr_in clientAddr;
        socklen_t clientAddrLen = sizeof(clientAddr);
        int client_sock = accept(tcp_sockfd,
                                 (struct sockaddr *) &clientAddr,
                                 &clientAddrLen);

        if (client_sock != -1) {
            // Store this client's TCP socket in the map

            std::thread client_thread([this, client_sock]() {
                //will send an event to the client to tell him his id
                //We need an header of 20 bytes with the type of event and the size of the data
                //the header will be like this : TCP:[10 bytes of size][5 bytes of content that will be the id]
                //it need to be filled with 0
                char header[20];
                std::string content = std::to_string(client_sock);
                std::string sizeStr = std::to_string(content.length());
                sizeStr.insert(sizeStr.begin(), 10 - sizeStr.length(), '0');
                content.insert(content.begin(), 5 - content.length(), '0');
                std::string headerStr = "TCP:" + sizeStr + content;
                ssize_t sentBytes = send(client_sock, headerStr.c_str(), headerStr.length(), 0);
                if (sentBytes == -1) {
                    perror("send");
                }
                TCPsocketQueue.push(client_sock);
                //now prepapre the map that link the client id, the tcp socket and the udp address, and the entity id
                //the entity id will be link later after receive the udp message

                while (true) {

                    // Step 1: Read Header (assume header is 15 bytes: WDS:size (10 bytes))
                    char headerBuffer[20];
                    ssize_t bytes_received = recv(client_sock, headerBuffer, sizeof(headerBuffer) - 1, 0);
                    if (bytes_received == 0) {
                        close(client_sock);
                        break;
                    } else if (bytes_received < 0) {
                        // An error occurred
                        if (errno == EWOULDBLOCK || errno == EAGAIN) {
                            // These would be normal for a non-blocking socket; ignore and loop again.
                            continue;
                        }
                        perror("recv");
                        close(client_sock);
                        break;
                    } else if (bytes_received < 19) {
                        close(client_sock);
                        return;
                    }
                    headerBuffer[bytes_received] = '\0'; // Null-terminate based on the actual received size
                    std::string eventTypeStr = std::string(headerBuffer).substr(0, 3);

                    auto eventTypeIt = eventIdentifierMap.find(
                            eventTypeStr);
                    if (eventTypeIt == eventIdentifierMap.end()) {
                        close(client_sock);
                        return;
                    } else if (eventTypeIt != eventIdentifierMap.end()) {
                        int dataSize = std::stoi(std::string(headerBuffer).substr(4, 10));
                        //the 14 to 20 is the content of the event same as the size of the data it will be filled with 0
                        std::string content = std::string(headerBuffer).substr(14, 20);
                        EventType eventType = eventTypeIt->second;
                        Event newEvent;
                        newEvent.type = eventType;
                        //check if the event exist in the map
                        newEvent.content = content;
                        newEvent.TCPDataSize = dataSize;
                        newEvent.clientID = client_sock;
                        registry.eventBus.fireEvent(newEvent);
                    }
                }
            });

            client_thread.detach();

        } else {
            if (errno != EWOULDBLOCK && errno != EAGAIN) {
                // Handle error
                //get the pip of the program that hold the port
                std::cerr << "Error accepting TCP connection: "
                          << strerror(errno) << std::endl;
            }
        }

        char udp_buffer[1024];
        struct sockaddr_in udp_clientAddr;
        socklen_t udp_clientAddrLen = sizeof(udp_clientAddr);
        ssize_t recvLen = recvfrom(udp_sockfd, udp_buffer, 1024, 0,
                                   (struct sockaddr *) &udp_clientAddr,
                                   &udp_clientAddrLen);
        if (recvLen != -1) {
            udp_buffer[recvLen] = '\0';
            int clientID = -1;
            for (auto &client: clientInfoMap) {
                if (std::get<0>(client.second).sin_addr.s_addr == udp_clientAddr.sin_addr.s_addr &&
                    std::get<0>(client.second).sin_port == udp_clientAddr.sin_port) {
                    clientID = client.first;
                    break;
                }
            }
        std::vector<uint8_t> receivedMessageBytes(udp_buffer, udp_buffer + recvLen);
            if (clientID != -1) {
                // Client found in the map
                std::string receivedMessage = decodeToString(receivedMessageBytes);
                std::string eventTypeStr = receivedMessage.substr(0,
                                                                  receivedMessage.find(
                                                                          ":"));
                std::string eventContent = receivedMessage.substr(
                        receivedMessage.find(":") + 1);
                // Lookup the EventType based on the identifier
                auto eventTypeIt = eventIdentifierMap.find(
                        eventTypeStr);
                if (eventTypeIt != eventIdentifierMap.end()) {
                    EventType eventType = eventTypeIt->second;
                    Event newEvent;
                    newEvent.type = eventType;
                    //check if the event exist in the map
                    newEvent.content = std::string(eventContent);
                    newEvent.clientID = clientID;
                    newEvent.UDPAddr = udp_clientAddr;
                    registry.eventBus.fireEvent(newEvent);
                }
            } else {
                //special case if the content is UDPInit
                std::string receivedMessage = decodeToString(receivedMessageBytes);
                std::string eventTypeStr = receivedMessage.substr(0,
                                                                  receivedMessage.find(":"));
                std::string eventContent = receivedMessage.substr(
                        receivedMessage.find(":") + 1);
                // Lookup the EventType based on the identifier
                if (eventTypeStr == "UDP") {
                    //will fire
                    EventType eventType = UDPInit;
                    Event newEvent;
                    newEvent.type = eventType;
                    //check if the event exist in the map
                    newEvent.content = std::string(eventContent);
                    newEvent.clientID = -1;
                    newEvent.UDPAddr = udp_clientAddr;
                    registry.eventBus.fireEvent(newEvent);
                    //w
                }
            }
        }
    }

    void PostUpdate() override {
        while (!newPlayersQueue.empty()) {
            //now that the client is connected and we will creat a new entity for them and add it to the map
            //we will also send them the id of the entity
            int client_id = newPlayersQueue.front();
            newPlayersQueue.pop();
            entity_t newEntity = registry.spawn_entity();
            Client clientComponent;
            clientComponent.id = client_id;
            clientComponent.entity_id = newEntity;
            registry.add_component(client_id, std::move(clientComponent));
            registry.clientToEntityMap[client_id] = newEntity;
        }
        while (!newLobbiesQueue.empty()) {
            //now that the client is connected and we will creat a new entity for them and add it to the map
            //we will also send them the id of the entity
            contentIDLobbyCreation = newLobbiesQueue.front();
            newLobbiesQueue.pop();
            if (registry.clientToEntityMap.find(std::get<1>(contentIDLobbyCreation)) == registry.clientToEntityMap.end())
                return;
            // now check if the client is already in a lobby
            for (const auto &lobby: activeLobbies) {
                for (const auto &player: lobby.players) {
                    if (std::get<0>(player) == std::get<1>(contentIDLobbyCreation))
                        return;
                }
            }

            lobby newLobby;
            newLobby.id = std::get<2>(contentIDLobbyCreation);
            std::string content = std::get<0>(contentIDLobbyCreation);
            std::istringstream iss(content);
            std::string folderPath, songName, artistName;
            std::getline(iss, folderPath, ' ');
            std::getline(iss, songName, ' ');
            std::getline(iss, artistName, ' ');
            newLobby.name = "Lobby_" + std::to_string(newLobby.id);
            newLobby.artistName = artistName;
            newLobby.songName = songName;
            newLobby.players.push_back({std::get<1>(contentIDLobbyCreation),
                                        registry.clientToEntityMap[std::get<1>(contentIDLobbyCreation)], false});
            activeLobbies.push_back(newLobby);
            // Send lobby creation success event
            std::string message = "LC:" + std::to_string(newLobby.id) + " " + newLobby.name + " " + newLobby.artistName + " " + newLobby.songName;
            //add a send to client using the event.clientID
            struct sockaddr_in &clientAddr = getUDPAdrWithUniqueId(std::get<1>(contentIDLobbyCreation));
            sendto(udp_sockfd, message.c_str(), message.length(), 0, (struct sockaddr *) &clientAddr,
                   sizeof(clientAddr));
            //will then send to say that the player who creaed the lobby joined it PJL

        }

        while (!sendLobbyListQueue.empty()) {
            int client_id = sendLobbyListQueue.front();
            sendLobbyListQueue.pop();
            std::string message = "LSC:";
            for (auto &lobby: activeLobbies) {
                if (lobby.isGameStarted) {
                    continue;
                }
                //check if there is at least one player in the lobby
                if (lobby.musicStored == false )
                    continue;
                message += std::to_string(lobby.id) + " " + lobby.name + " " + lobby.artistName + " " + lobby.songName + " " + std::to_string(lobby.players.size()) + "|";
            }
            //add a send to client using the event.clientID
            struct sockaddr_in &clientAddr = getUDPAdrWithUniqueId(client_id);
            //send message to client
            sendto(udp_sockfd, message.c_str(), message.length(), 0, (struct sockaddr *) &clientAddr,
                   sizeof(clientAddr));
        }
        while (!startGameQueue.empty()) {
            int client_id = startGameQueue.front();
            startGameQueue.pop();
            struct sockaddr_in &clientAddr = getUDPAdrWithUniqueId(client_id);
            std::string message = "GS:";
            sendto(udp_sockfd, message.c_str(), message.length(), 0, (struct sockaddr *) &clientAddr,
                   sizeof(clientAddr));
        }
    }
};

#endif /* !SERVERSYSTEM_HPP_ */