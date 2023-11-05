/*
** EPITECH PROJECT, 2024
** Rtype
** File description:
** MenuSystem
*/

/**
 * MenuSystem handles the game menu logic and UI.
 *
 * It loads menu assets and database.
 * Spawns UI entities like buttons, panels, and backgrounds.
 * Handles menu navigation and button callbacks.
 * Triggers events on button clicks and menu actions.
 * Transitions between menu screens.
 *
 * Usage:
 * - Call InitializeMainMenu() and InitializeLobby() to set up menus.
 * - Implement button click handlers.
 * - Transition menus by setting State component's state enum.
 *
 * Parameters:
 * - registry: Reference to ECS registry.
 * - assetManager: Reference to asset manager.
 *
 * Components used:
 * - State: Tracks current menu state.
 * - Position: Used to position menu entities.
 *
 * Events:
 * - IndexMoved: Navigates between menu items.
 * - Button click events: Triggered by buttons.
 * - State change events: Change menu screens.
*/

#ifndef MENUSYSTEM_HPP
#define MENUSYSTEM_HPP

#include "ISystem.hpp"
#include "Spawner.hpp"
#include "SceneParsing.hpp"
#include <SFML/Graphics.hpp>

struct lobbyInfo {
    std::string name;
    std::string songName;
    std::string artistName;
    int nbPlayer;
    int nbPlayerReady;
    int id;
};

struct skinInfo {
    std::string name;
    std::string path;  // path to the skin asset
};


class MenuSystem : public ISystem<State> {
private:
    Registry &registry;
    float &deltaTime;
    std::vector<entity_t> beatmapEntities;
    std::vector<std::pair<entity_t, bool>> gridEntities;
    std::map<int, std::pair<entity_t, bool>> clientToLobbyIdMap;
    std::map<int, int> scoreMap;
    int selectedPanelIndex = 0;
    AssetManager &assetManager;
    Database database;
    bool gameStartedFlag = false;
    bool lobbyCreatedFlag = false;
    bool lobbySelectionCreatedFlag = false;
    bool lobbyJoinedFlag = false;
    bool loadScoreLobby = false;
    bool loadMainMenuFlag = false;
    int lobbySelectedIndex = 0;
    bool SendWavData = false;
    std::vector<lobbyInfo> lobbyList;
    std::string lobbyJoinedData;
    std::string lockTime;
    int idUdp = -1;

public:

    std::string const getSystemName() override {
        return "MenuSystem";
    }
    MenuSystem(Registry &registry, AssetManager &assetManager, float &deltaTime) : registry(registry), assetManager(assetManager),deltaTime(deltaTime) {
        addlisteners(registry);
    }
    void addNoneLocalPlayer(){
        registry.printFreeIds();
        entity_t localPlayerEntity =  registry.clientToEntityMap[-42];
        if (localPlayerEntity == -1){
            return;
        }
        // Access the GameConfig component to get the skin name
        auto &gameConfigOpt = registry.get_components<GameConfig>()[registry.stateEntity];
        if (!gameConfigOpt.has_value()) {
            std::cerr << "GameConfig not set. Exiting function." << std::endl;
            return;
        }
        auto &gameConfig = *gameConfigOpt;
        std::string skinName = gameConfig.playerSkin;
        // Access and update the Drawable component of the local player entity
        auto &drawableComponents = registry.get_components<Drawable>();
        auto &drawableOpt = drawableComponents[localPlayerEntity];
        if (!drawableOpt.has_value()) {
            std::cerr << "Local player entity " << localPlayerEntity << " has no Drawable component" << std::endl;
            return;
        }
        auto &drawable = *drawableOpt;
        drawable.textureName = skinName;
        for (auto& client: clientToLobbyIdMap){
                //replace the entity with the new one
                //check if we are checking yourself
                if (client.first == idUdp)
                    continue;
                std::get<0>(client.second) = spawnNoneLocalPlayer (registry,MyRect{0,0,176,96});
                registry.clientToEntityMap[client.first] = std::get<0>(client.second);
        }
        registry.clientToEntityMap[idUdp] = localPlayerEntity;
        registry.clientToEntityMap.erase(-42);
        // now that we have the id of all the entity and the id, we will sync their position
        // to do so we will use the clklient udp id (the key of the map);
        float multiplier = 1.0f;
        auto &position = registry.get_components<Position>();
        for (auto& client: clientToLobbyIdMap){
            auto &positionOpt = position[registry.clientToEntityMap[client.first]];
            if (!positionOpt.has_value()) {
                std::cerr << "Entity " << registry.clientToEntityMap[client.first] << " has no position component" << std::endl;
                continue;
            }
            auto &pos = *position[registry.clientToEntityMap[client.first]];
            pos.x = 100;
            pos.y = 200 * multiplier;
            pos.prevX = pos.x;
            pos.prevY = pos.y;
            multiplier += 1.0f;
            registry.printFreeIds();
            //we will add a text drawable to the entity to display the name of the player
            spawnFollowerText(registry.clientToEntityMap[client.first], registry, "P" + std::to_string(client.first), 50, 0);

        }

    }

    void addlisteners(Registry &registry) {
        registry.eventBus.addListener(PlayerAdded, [this](const Event& event) {
            idUdp = std::stoi(event.content);
        });
        registry.eventBus.addListener(IndexMoved, [this, &registry](const Event &event) {
            int oldIndex = selectedPanelIndex;
            selectedPanelIndex += atoi(event.content.c_str());
            if (selectedPanelIndex < 0)
                selectedPanelIndex = 0;
            if (selectedPanelIndex > beatmapEntities.size() - 1)
                selectedPanelIndex = beatmapEntities.size() - 1;

            // Only proceed if the index actually changed
            if (oldIndex == selectedPanelIndex) {
                return;
            }

            // Calculate the distance to move based on the initial panel spacing
            int distance = 1000;  // Initial distance between each panel

            // Calculate the offset based on the old and new selected indices
            int offset = (selectedPanelIndex - oldIndex) * distance;

            // Update all beatmap panels to move them to the left or right
            for (entity_t entity: beatmapEntities) {
                auto &position = registry.get_components<Position>();
                auto &positionOpt = position[entity];
                if (!positionOpt) {
                    std::cerr << "Beatmap panel entity " << entity << " has no position component" << std::endl;
                    continue;
                }
                auto &pos = *positionOpt;

                // Move each panel by the calculated offset
                pos.x -= offset;

                // Optional: Add boundary checks here
            }
        });
        registry.eventBus.addListener(LobbyJoined, [this, &registry](const Event &event) {
            //set the select lobby index to the lobby joined
            //tokenize the content of the event with :
            lobbyJoinedData = event.content;
            lobbyJoinedFlag = true;
        });
        registry.eventBus.addListener(LobbyCreated, [this, &registry](const Event &event) {
            //set the select lobby index to the lobby created
            //get the content bewteen LC: and the first " "
            lobbySelectedIndex = std::stoi(event.content.substr(0, event.content.find(" ")));
            lobbyCreatedFlag = true;
            //need to kill all the beatmap panel, button and background
        });
        registry.eventBus.addListener(LobbySelectionCreated, [this, &registry](const Event &event) {
            //use the content of the event to populate lobby list
            lobbyList.clear();
            clientToLobbyIdMap.clear();
            gridEntities.clear();
            std::string message = event.content;
            std::string delimiter = "|";
            size_t pos = 0;
            std::string token;
            while ((pos = message.find(delimiter)) != std::string::npos) {
                token = message.substr(0, pos);
                message.erase(0, pos + delimiter.length());  // Erase the processed part

                std::string delimiter2 = " ";
                size_t pos2 = 0;
                std::string token2;
                std::vector<std::string> tokens;
                while ((pos2 = token.find(delimiter2)) != std::string::npos) {
                    tokens.push_back(token.substr(0, pos2));
                    token.erase(0, pos2 + delimiter2.length());
                }
                tokens.push_back(token);
                lobbyInfo lobby;
                lobby.id = std::stoi(tokens[0]);
                lobby.name = tokens[1];
                lobby.songName = tokens[3];
                lobby.artistName = tokens[2];
                lobby.nbPlayer = std::stoi(tokens[4]);
                lobbyList.push_back(lobby);
            }
            lobbySelectionCreatedFlag = true;
            auto &stateOpt = registry.get_components<State>()[registry.stateEntity];
            if (!stateOpt.has_value()) {
                std::cerr << "Entity " << registry.stateEntity << " has no state component" << std::endl;
                return;
            }
            auto &state = *stateOpt;
            state.state = LOBBYLISTPRELOAD;
            //need to kill all the beatmap panel, button and background
        });
        registry.eventBus.addListener(LobbyDestroyed, [this, &registry](const Event &event) {
            //will go back to the menu and reload the lobby list
            Event newEvent;
            newEvent.type = EventType::AskDataListLobby;
            newEvent.content = "-1";
            newEvent.clientID = 0;
            registry.eventBus.fireEvent(newEvent);
            selectedPanelIndex = 0;
        });
        registry.eventBus.addListener(LobbyLeft, [this, &registry](const Event &event) {
            //will go back to the menu and reload the lobby list
            //delete every entity that is in the lobby
            Event newEvent;
            newEvent.type = EventType::AskDataListLobby;
            newEvent.content = "-1";
            newEvent.clientID = 0;
            registry.eventBus.fireEvent(newEvent);
            selectedPanelIndex = 0;
        });
        registry.eventBus.addListener(GameStarted, [this](const Event &event) {
            std::vector<std::string> tokens = tokenize(event.content, "|");
            //this is our id we will use it for the nonlocal spawn later
            //we will received the id and a time stamp
            //token 1 is the time stamp
            //we need to store it and use it to sync the game
            lockTime = tokens[1];
            gameStartedFlag = true;

        });
        registry.eventBus.addListener(BackToMenu, [this, &registry](const Event &event) {
            loadMainMenuFlag = true;
        });
        registry.eventBus.addListener(LobbyPlayerReady, [this, &registry](const Event &event) {
            // we will check if the player id is present in the map.
            // if it is then we will change the depth of the entity to 4
            // so it will be rendered above the grid
            int clientId = stoi(event.content);
            if (clientToLobbyIdMap.find(clientId) != clientToLobbyIdMap.end()) {
                //change the depth of the entity to 4
                auto &depth = registry.get_components<Depth>()[clientToLobbyIdMap[clientId].first];
                if (!depth.has_value()) {
                    std::cerr << "Entity " << clientToLobbyIdMap[clientId].first << " has no depth component"
                              << std::endl;
                    return;
                }
                auto &depthComp = *depth;
                depthComp.z = 4;

            }
        });

        registry.eventBus.addListener(PlayerJoinedLobby, [this, &registry](const Event &event) {
            //we will receive the name of the lobby (we will store it to save time to the server later) and the id of the player
            //we will add entities into the lobby to represent the player
            std::string message = event.content;
            std::vector <std::string> tokens = tokenize(message, ":");
            //check if already in the map if not then will be a new player
            int clientId = stoi(tokens[0]);
            // Check if player already exists in the lobby
            if (clientToLobbyIdMap.find(clientId) != clientToLobbyIdMap.end()) {
                return;
            }
            for (auto & gridEntity : gridEntities) {
                if (!gridEntity.second) {
                    gridEntity.second = true;
                    clientToLobbyIdMap[clientId].first = gridEntity.first;
                    clientToLobbyIdMap[clientId].second = false;
                    break;
                }
            }
        });
        registry.eventBus.addListener(playerEndScore, [this, &registry](const Event &event) {
            //will received a serialized string of the score of the player
            //will need to deserialize it and store it in a map
            std::string message = event.content;
            std::vector<std::string> tokens = tokenize(message, ":");

            for (int i = 0; i < tokens.size() - 1; i += 2) {
                int playerID = std::stoi(tokens[i]);
                int playerScore = std::stoi(tokens[i + 1]);
                scoreMap[playerID] = playerScore;
            }
            loadScoreLobby = true;
        });
    }


    void InitializeMainMenu(Registry &registry) {
        database.updateDatabase("./config/Database.cfg");
        database.print();
        selectedPanelIndex = 0;
        loadMenuAsset(assetManager, database);
        for (int i = 0; i < database.getNbBeatmaps(); ++i) {
            beatmapEntities.push_back(spawnBeatmapPanel(registry, database, i));
        }


        std::vector<std::function<void()>> buttonCallbacks =
                {
                        [this, &registry]() {
                            Event newEvent;
                            newEvent.type = EventType::GameExited;
                            newEvent.content = "-1";
                            newEvent.clientID = 0;
                            registry.eventBus.fireEvent(newEvent);
                        },
                        [this, &registry]() {
                            Event newEvent;
                            newEvent.type = EventType::IndexMoved;
                            newEvent.content = "-1";
                            newEvent.clientID = 0;
                            registry.eventBus.fireEvent(newEvent);
                        },
                        [this, &registry]() {
                            Event newEvent;
                            newEvent.type = EventType::IndexMoved;
                            newEvent.content = "1";
                            newEvent.clientID = 0;
                            registry.eventBus.fireEvent(newEvent);
                        },
                        [this, &registry]() {
                            Event newEvent;
                            newEvent.type = EventType::AskDataCreationLobby;
                            //regroupe name of the song and artiste name
                            newEvent.content = database.getBeatmap(selectedPanelIndex).getFolderPath() + " " +
                                               database.getBeatmap(selectedPanelIndex).getName() + " " +
                                               database.getBeatmap(selectedPanelIndex).getArtist();
                            newEvent.clientID = 0;
                            registry.eventBus.fireEvent(newEvent);
                        },
                        [this, &registry]() {
                            Event newEvent;
                            newEvent.type = EventType::AskDataListLobby;
                            newEvent.content = "-1";
                            newEvent.clientID = 0;
                            registry.eventBus.fireEvent(newEvent);
                        },
                        [this, &registry]() {
                            Event newEvent;
                            newEvent.type = EventType::ClientTryConnect;
                            newEvent.content = "-1";
                            newEvent.clientID = 0;
                            registry.eventBus.fireEvent(newEvent);
                        }
                };
        std::vector<std::string> buttonLabels = {
                "Exit",
                "Left",
                "Right",
                "Start",
                "Lobby List",
                "Connect"
        };
        // Initialize buttons
        for (int i = 0; i < 6; ++i) {
            std::string textureName = "Btn0" + std::to_string(i + 1);
            MyRect rect{0, 0, 155, 42};  // Modify as needed
            std::string text = buttonLabels[i];
            MyVector2 scale{1, 1};  // Modify as needed
            MyVector2 textOffset{0, 0};  // Modify as needed
            MyVector2 textScale{1, 1};  // Modify as needed
            float x = 200 + i * 200;
            float y = 900;
            MyVector2 position{x, y};
            spawnButton(registry, rect, textureName, text, "font", scale, textOffset, textScale, buttonCallbacks[i],
                        position);
        }
        spawnBackgroundLayer(registry, Behavior{"Fixed"}, "background", 0, 0, MyRect{0, 0, 1920, 1080}, MyVector2{1, 1});

    }
    void InitializeLobby(Registry &registry, bool isHost) {
        loadSkins(assetManager);
        std::vector<std::function<void()>> buttonCallbacks = {
                [this, &registry]() {
                    Event newEvent;
                    newEvent.type = EventType::AskLobbyDestroyed;
                    newEvent.content = "-1";
                    newEvent.clientID = 0;
                    registry.eventBus.fireEvent(newEvent);
                },
                [this, &registry]() {
                    Event newEvent;
                    newEvent.type = EventType::AskLobbyLeft;
                    newEvent.content = "-1";
                    newEvent.clientID = 0;
                    registry.eventBus.fireEvent(newEvent);
                },
                [this, &registry]() {
                    Event newEvent;
                    newEvent.type = EventType::LobbyMessageSent;
                    newEvent.content = "-1";
                    newEvent.clientID = 0;
                    registry.eventBus.fireEvent(newEvent);
                },
                [this, &registry]() {
                    Event newEvent;
                    newEvent.type = EventType::GameStarted;
                    newEvent.content = "-1";
                    newEvent.clientID = 0;
                    registry.eventBus.fireEvent(newEvent);
                },
                [this, &registry]() {
                    Event newEvent;
                    newEvent.type = EventType::FFTStart;
                    newEvent.content = "-1";
                    newEvent.clientID = 0;
                    registry.eventBus.fireEvent(newEvent);
                }
        };
        spawnBackgroundLayer(registry, Behavior{"Fixed"}, "background", 0, 0, MyRect{0, 0, 1920, 1080}, MyVector2{1, 1});
        entity_t background = spawnDrawable(registry, MyRect{0, 0, 922, 599}, "LobbyBackground", MyVector2{1.5, 1.5}, 300, 125, 1);
        entity_t BackButton;
        if (isHost) {
            BackButton = spawnButton(registry, MyRect{0, 0, 225, 50}, "LobbyButtonLeft", "Leave", "font", MyVector2{0.5, 0.5}, MyVector2{0, 0}, MyVector2{1, 1}, buttonCallbacks[0], MyVector2{200, 900});
        } else {
            BackButton = spawnButton(registry, MyRect{0, 0, 225, 50}, "LobbyButtonLeft", "Leave", "font", MyVector2{0.5, 0.5}, MyVector2{0, 0}, MyVector2{1, 1}, buttonCallbacks[1], MyVector2{200, 900});
        }
        registry.add_component<Follower>(BackButton, Follower{background, 50, 820});
        //button to start the game
        entity_t StartButton = spawnButton(registry, MyRect{0, 0, 225, 50}, "LobbyButtonRight", "Start", "font", MyVector2{0.5, 0.5}, MyVector2{0, 0}, MyVector2{1, 1}, buttonCallbacks[4], MyVector2{200, 900});
        registry.add_component<Follower>(StartButton, Follower{background, 1150, 820});
        float x = 310;
        float y = 200;
        int i = 0;
        for (const auto &texture : assetManager.getTextures()) {
            if (texture.first.compare(0, 5, "skin_") == 0) {  // Check for "skin_" prefix
                std::string textureName = texture.first;
                MyRect rect{0, 0, 208, 29};  // Modify as needed
                std::string text = textureName.substr(5, textureName.size() - 5);
                MyVector2 scale{1, 1};  // Modify as needed
                MyVector2 textOffset{0, 0};  // Modify as needed
                MyVector2 textScale{0.8f, 0.8f};  // Modify as needed
                y = 200 + i * 50;
                i++;
                MyVector2 position{x, y};
                spawnButton(registry, rect, "LobbyHolderBig", text, "font", scale, textOffset, textScale, [&, textureName]() {
                    Event event;
                    event.type = SkinSelected;
                    event.content = textureName;  // Send the skin name as the content of the event
                    registry.eventBus.fireEvent(event);
                },
                position);
            }
        }
        //the big cover of the song
        spawnFollowerCover(background, registry, MyRect{0, 0, 500, 500},
                           database.getBeatmap(selectedPanelIndex).getFolderPath(), MyVector2{1000, 150});
        //the 2 holder for the artist name and name of the song
        entity_t TitleHolder = spawnDrawable(registry, MyRect{0, 0, 208, 29}, "LobbyHolderBig", MyVector2{1, 1}, 300,125, 2);
        registry.add_component<Follower>(TitleHolder, Follower{background, 300, 100});
        spawnFollowerText(TitleHolder, registry, database.getBeatmap(selectedPanelIndex).getArtist(), 0, 0);
        entity_t ArtistHolder = spawnDrawable(registry, MyRect{0, 0, 183, 29}, "LobbyHolderSmall", MyVector2{1, 1}, 300,125, 2);
        spawnFollowerText(ArtistHolder, registry, database.getBeatmap(selectedPanelIndex).getName(), 0, 0);
        registry.add_component<Follower>(ArtistHolder, Follower{background, 300 + 208 + 10, 100});
        entity_t gridPlaform = spawnDrawable(registry, MyRect{0, 0, 109, 109}, "LobbyGridHolder", MyVector2{1, 1}, 300, 125, 3);
        registry.add_component<Follower>(gridPlaform, Follower{background, 300, 300});
        //will add then to gridEntities
        x = 0;
        y = 0;
        for (int i = 0; i < 4; i++) {
            //as its will be an holder of 2 by 2 icons
            entity_t grid = spawnDrawable(registry, MyRect{0, 0, 21, 23}, "lobbyPlayerIcon", MyVector2{1, 1}, 300, 125, 0);
            if (i < 2) {
                x = 0 + i * 50;
                y = 0;
                registry.add_component<Follower>(grid, Follower{gridPlaform, x, y});
            } else {
                x = 0 + (i - 2) * 50;
                y = 50;
                registry.add_component<Follower>(grid, Follower{gridPlaform, x, y});
            }
            gridEntities.push_back(std::make_pair(grid, false));
        }
        entity_t SkinPanel = spawnDrawable(registry, MyRect{0, 0, 852, 527}, "MainPanel01", MyVector2{0.35f, 0.35f}, 0, 0,1);
        spawnFollowerText(SkinPanel, registry, "Skin Preview", 105, 103);
        registry.add_component<Follower>(SkinPanel, Follower{background, 1055, 600});
        spawnDrawable(registry, MyRect{0, 0, 500, 500}, "skin_default", MyVector2{1, 1}, 1415, 750, 1);
        registry.eventBus.addListener(GameStarted, [this, &registry](const Event& event) {
            auto &gameConfigOpt = registry.get_components<GameConfig>()[registry.stateEntity];
            if (!gameConfigOpt.has_value()) {
                std::cerr << "GameConfig not set. Exiting Skin Change." << std::endl;
            } else {
                auto &config = *gameConfigOpt;
                auto &componentsPosition = registry.get_components<Position>();
                for (entity_t entity = 0; entity < componentsPosition.size(); ++entity) {
                    if (auto posOpt = componentsPosition[entity]) {
                        auto &pos = *posOpt;
                        if (pos.x == 1415 && pos.y == 750) {
                            config.playerSkin = registry.get_components<Drawable>()[entity]->textureName;
                            break;
                        }
                    }
                }
            }
            gameStartedFlag = true;
        });

        registry.eventBus.addListener(SkinSelected, [this, &registry](const Event &event) {
            std::string textureName = event.content;
            auto &componentsPosition = registry.get_components<Position>();
            for (entity_t entity = 0; entity < componentsPosition.size(); ++entity) {
                if (auto posOpt = componentsPosition[entity]) {
                    auto &pos = *posOpt;
                    if (pos.x == 1415 && pos.y == 750)
                        registry.queue_for_deletion(entity);
                }
            }
            registry.delete_queued_entities();
            spawnDrawable(registry, MyRect{0, 0, 500, 500}, textureName, MyVector2{1, 1}, 1415, 750, 1);
        });
    }

    void initializeLobbyScore(Registry &registry)
    {
        //unload the texture of the game to load
        loadMenuAsset(assetManager, database);
        spawnBackgroundLayer(registry, Behavior{"Fixed"}, "background", 0, 0, MyRect{0, 0, 1920, 1080},
                             MyVector2{1, 1});
        entity_t background = spawnDrawable(registry, MyRect{0, 0, 922, 599}, "LobbyBackground", MyVector2{1.5, 1.5},
                                            300, 125, 1);
        entity_t backButton = spawnButton(registry, MyRect{0, 0, 225, 50}, "LobbyButtonLeft", "Back", "font",
                                          MyVector2{0.5, 0.5}, MyVector2{0, 0}, MyVector2{1, 1},
                                          [&]() {
                                              // Back button callback
                                              Event event;
                                              event.type = BackToMenu;
                                              registry.eventBus.fireEvent(event);
                                          },
                                          MyVector2{200, 900});
        registry.add_component<Follower>(backButton, Follower{background, 50, 800});

        float multiplier = 1.0f;
        float x, y;
        int score = 0;
        unsigned int fontSize = 20;

        for (auto const& playerScore : scoreMap) {
            x = 50;
            y = 200 + 50 * multiplier;
            entity_t nameHolder = spawnDrawable(registry, MyRect{0, 0, 183, 29}, "LobbyHolderSmall", MyVector2{1.5, 1.5}, x, y, 1);
            registry.add_component<Follower>(nameHolder, Follower{background, 250, 300 + 50 * multiplier});
            std::string name = "Player " + std::to_string(playerScore.first) + " : " + std::to_string(playerScore.second);
            spawnFollowerText(nameHolder, registry, name, 0, -25, fontSize);
            multiplier += 1.0f;
            score += playerScore.second;
        }
        //add a total score text
        entity_t totalScoreHolder = spawnDrawable(registry, MyRect{0, 0, 183, 29}, "LobbyHolderSmall", MyVector2{1.5, 1.5}, x, y, 1);
        registry.add_component<Follower>(totalScoreHolder, Follower{background, 200, 200});
        spawnFollowerText(totalScoreHolder, registry, "Total Score: " + std::to_string(score), 0, -25, fontSize);
        // we will then clear the map of entities and score map for the next scene
        scoreMap.clear();
    }

    void InitializeLobbySelectionScene(Registry &registry) {
        spawnBackgroundLayer(registry, Behavior{"Fixed"}, "background", 0, 0, MyRect{0, 0, 1920, 1080},
                             MyVector2{1, 1});
        entity_t background = spawnDrawable(registry, MyRect{0, 0, 922, 599}, "LobbyBackground", MyVector2{1.5, 1.5},
                                            300, 125, 1);
        entity_t backButton = spawnButton(registry, MyRect{0, 0, 225, 50}, "LobbyButtonLeft", "Back", "font",
                                          MyVector2{0.5, 0.5}, MyVector2{0, 0}, MyVector2{1, 1},
                                          [&]() {
                                              // Back button callback
                                              Event event;
                                              event.type = BackToMenu;
                                              registry.eventBus.fireEvent(event);
                                          },
                                          MyVector2{200, 900});

        registry.add_component<Follower>(backButton, Follower{background, 50, 800});

        // Create join buttons
        for (int i = 0; i < lobbyList.size(); i++) {

            std::string text = lobbyList[i].name + " " + lobbyList[i].songName +
                               std::to_string(lobbyList[i].nbPlayer) + "/" + std::to_string(lobbyList[i].nbPlayerReady);
            float yPos = 250 + i * 100;
            if (database.isBeatmapExist(lobbyList[i].songName)) {
                entity_t button = spawnButton(registry, MyRect{0, 0, 183, 29}, "LobbyHolderSmall", text, "font",
                                              MyVector2{0.5, 0.5},
                                              MyVector2{0, 0}, MyVector2{1, 1},
                                              [&, i]() {
                                                  // Join button callback
                                                  Event event;
                                                  event.type = LobbyJoinRequested;
                                                  std::string signature = std::to_string(lobbyList[i].id);
                                                  signature.insert(signature.begin(), 5 - signature.length(), '0');
                                                  event.content = signature;
                                                  registry.eventBus.fireEvent(event);
                                              },
                                              MyVector2{1000, yPos});
                registry.add_component<Follower>(button, Follower{background, 10, yPos});
            } else {
                entity_t button = spawnButton(registry, MyRect{0, 0, 183, 29}, "LobbyHolderSmall", text, "font",
                                              MyVector2{0.5, 0.5},
                                              MyVector2{0, 0}, MyVector2{1, 1},
                                              [&, i]() {
                                                  // Join button callback
                                                  Event event;
                                                  event.type = WavDataAsked;
                                                  event.content = std::to_string(i + 1) + " " + lobbyList[i].name;
                                                  registry.eventBus.fireEvent(event);
                                              },
                                              MyVector2{1000, yPos});
                registry.add_component<Follower>(button, Follower{background, 10, yPos});
            }
        }
    }

    void operator()(entity_t entity, sparse_array<State> &states) override {
        auto &stateOpt = states[entity];
        if (!stateOpt.has_value()) {
            std::cerr << "Entity " << entity << " has no state component" << std::endl;
            return;
        }
        auto &state = *states[entity];
        switch (state.state) {
            case MENU:
                break;
            case LOBBY:
                if (!SendWavData) {
                    fireWaveData();
                    SendWavData = true;
                }
                break;
            case MENUPRELOAD:
                InitializeMainMenu(registry);
                state.state = MENU;
                break;
            case LOBBYLIST:
                break;
            case LOBBYLISTPRELOAD:
                state.state = LOBBYLIST;
                break;
            case LOBBYSCORE:
                break;
            case LOBBYSCOREPRELOAD:
                registry.clearGameState();
                initializeLobbyScore(registry);
                registry.stateChanged = true;
                state.state = LOBBYSCORE;
                break;
            case EXIT:
                break;
            default:
                break;
        }
    }

    void PostUpdate() override {
        if (lobbyCreatedFlag) {
            registry.clearGameState();
            registry.clearInactiveEntities();

            //clear the map of entities of the menu
            beatmapEntities.clear();
            std::string songName = database.getBeatmap(selectedPanelIndex).getFolderPath();
            registry.setPath("./asset/Beatmaps/" + songName);
            InitializeLobby(registry, true);
            // we will try to send the .wav data to the server at this moment for testing purpose
            lobbyCreatedFlag = false;
            SendWavData = false;
            registry.stateChanged = true;
            auto &stateOpt = registry.get_components<State>()[registry.stateEntity];
            if (!stateOpt.has_value()) {
                std::cerr << "Entity " << registry.stateEntity << " has no state component" << std::endl;
                return;
            }
            auto &state = *stateOpt;
            state.state = LOBBY;
        }
        if (lobbyJoinedFlag) {
            registry.clearGameState();
            registry.clearInactiveEntities();
            std::vector<std::string> tokens = tokenize(lobbyJoinedData, ":");
            std::string songNameTmp = tokens[0];
            std::string nextSongName = tokens[4];
            int nbPlayer = std::stoi(tokens[5]);
            //add the 0 to the string to get the name of the song
            songNameTmp.insert(songNameTmp.begin(), 5 - songNameTmp.length(), '0');
            //will execut a python scrypt that will add the song to the database AddToDatabase.py
            std::filesystem::path path = std::filesystem::current_path();
            //will now need to rename the path + songNameTmp.wav to path + nextSongName.wav
            //before doing all the script check if their is already a folder in path + nextSongName
            if (!std::filesystem::exists(path.string() + "/asset/Beatmaps/" + nextSongName)) {
                //folder does not exist
                std::string command = "mv " + path.string() + "/" + songNameTmp + ".wav " + path.string() + "/" + nextSongName + ".wav";
                system(command.c_str());
                //check if the file is present with the new name
                ifstream f(path.string() + "/" + nextSongName + ".wav");
                command = "python3 AddToDatabase.py " + path.string() + "/" + nextSongName + ".wav";
                //will now need to update the database a
                system(command.c_str());
                //rm the .wav used
                command = "rm " + path.string() + "/" + nextSongName + ".wav";
                system(command.c_str());
                selectedPanelIndex = database.updateDatabase(path.string() + "/config/Database.cfg");
            } else {
                selectedPanelIndex = database.getBeatmapByName(nextSongName);
            }
            loadMenuAsset(assetManager, database);
            registry.setPath(path.string() + "/asset/Beatmaps/" + nextSongName);
            InitializeLobby(registry, false);
            //the 5tk indicate the number of player in the lobby we will do 5 + token{5] value to attribute grid entities to an client id
            int counter = 0;
            for (int i = 0; counter < nbPlayer; i += 2) {
                clientToLobbyIdMap[stoi(tokens[6 + i])].first = gridEntities[counter].first;
                if (stoi(tokens[6 + i + 1]) == 1) {
                    //the player is ready
                    auto &depth = registry.get_components<Depth>()[gridEntities[counter].first];
                    if (!depth.has_value()) {
                        std::cerr << "Entity " << gridEntities[counter].first << " has no depth component" << std::endl;
                        return;
                    }
                    auto &depthComp = *depth;
                    depthComp.z = 4;
                    gridEntities[counter].second = true;
                }
                counter++;
            }

            // we will try to send the .wav data to the server at this moment for testing purpose
            SendWavData = true;
            lobbyJoinedFlag = false;
            registry.stateChanged = true;
            auto &stateOpt = registry.get_components<State>()[registry.stateEntity];
            if (!stateOpt.has_value()) {
                std::cerr << "Entity " << registry.stateEntity << " has no state component" << std::endl;
                return;
            }
            auto &state = *stateOpt;
            state.state = LOBBY;
        }

        if (gameStartedFlag) {
            registry.clearGameState();
            loadAllTexture(assetManager);
            //check the number of entities
            initializeGameEntitiesFromConfig(registry, "Scene/GameScene.cfg");
            addNoneLocalPlayer();
            registry.stateChanged = true;
            gameStartedFlag = false;
            //change state in registry
            auto &stateOpt = registry.get_components<State>()[registry.stateEntity];
            if (!stateOpt.has_value()) {
                std::cerr << "Entity " << registry.stateEntity << " has no state component" << std::endl;
                return;
            }
            //
            auto &state = *stateOpt;
            state.state = GameState::GAME;
            //now we will wait until the time stamp received by the server is the same as the one of the client
            std::time_t result = std::time(nullptr);
            std::string lockTimeStr = std::to_string(result);
            //lockTime is the timestand received we need to wait until the time is the same as the one of the client
            while (lockTime != lockTimeStr) {
                result = std::time(nullptr);
                lockTimeStr = std::to_string(result);
            }
            //when ended fire event for FFTPlay
            Event event;
            event.type = EventType::FFTPlay;
            event.content = "-1";
            event.clientID = 0;
            registry.eventBus.fireEvent(event);
            //we will set elapsed time to 0
            deltaTime = 0;
        }
        if (loadScoreLobby){
            registry.clearGameState();
            registry.clearInactiveEntities();

            auto &stateOpt = registry.get_components<State>()[registry.stateEntity];
            if (!stateOpt.has_value()) {
                std::cerr << "Entity " << registry.stateEntity << " has no state component" << std::endl;
                return;
            }
            auto &stateComp = *stateOpt;
            initializeLobbyScore(registry);
            stateComp.state = GameState::LOBBYSCORE;
            registry.stateChanged = true;
            loadScoreLobby = false;
            clientToLobbyIdMap.clear();
            gridEntities.clear();
            registry.clientToEntityMap.clear();
        }
        if (lobbySelectionCreatedFlag) {
            registry.clearGameState();
            registry.clearInactiveEntities();
            beatmapEntities.clear();
            InitializeLobbySelectionScene(registry);
            lobbySelectionCreatedFlag = false;
            registry.stateChanged = true;
        }
        if (loadMainMenuFlag){
            registry.clearGameState();
            registry.clearInactiveEntities();
            auto &stateOpt = registry.get_components<State>()[registry.stateEntity];
            if (!stateOpt.has_value()) {
                std::cerr << "Entity " << registry.stateEntity << " has no state component" << std::endl;
                return;
            }
            auto &state = *stateOpt;
            state.state = GameState::MENUPRELOAD;
            loadMainMenuFlag = false;
            registry.stateChanged = true;
        }
    }

    void fireWaveData() {
        std::vector<uint8_t> wavData;
        std::string songName = database.getBeatmap(selectedPanelIndex).getFolderPath();
        std::string wavPath = "./asset/Beatmaps/" + songName + "/Song.wav";
        std::ifstream wavFile(wavPath, std::ios::binary);
        if (!wavFile.is_open()) {
            std::cerr << "Failed to open wav file" << std::endl;
            return;
        }
        std::vector<char> buffer(std::istreambuf_iterator<char>(wavFile), {});
        //need to file the vector with the data of the wav file
        wavData.insert(wavData.end(), buffer.begin(), buffer.end());
        wavFile.close();
        Event event;
        event.type = EventType::WavDataSent;
        event.content = std::to_string(lobbySelectedIndex);
        event.wavData = wavData;
        event.TCPDataSize = event.wavData.size();
        event.clientID = 0;
        registry.eventBus.fireEvent(event);
    }
};

#endif /* !MENUSYSTEM_HPP */
