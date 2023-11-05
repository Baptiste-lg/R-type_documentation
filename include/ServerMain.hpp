/*
** EPITECH PROJECT, 2024
** Rtype
** File description:
** serverMain
*/

/**
** serverMain
**
** This file contains functions related to initializing and running the server.
**
** Functions:
**
** - initializeGameEntitiesServer()
**   Initializes game entities on the server.
**
** - initializeGameServerSystems()
**   Initializes server systems like physics.
**
** - initializeDatabaseAndConfig()
**   Initializes database and config.
**
** - firstLoadingScreen()
**   Displays initial loading screen.
**
** - serverLoop()
**   Main server loop.
**
** - componentAdderMap
**   Map to add components to entities.
**
*/


#ifndef SERVERMAIN_HPP_
#define SERVERMAIN_HPP_
#include "Component.hpp"
#include "SparseArray.hpp"
#include "GUI/GUI.hpp"
#include "Spawner.hpp"
#include "TextureManager.hpp"
#include "Global.hpp"
#include "Database/Database.hpp"
#include "Config/Config.hpp"
#include "taskLoader.hpp"

void initializeGameEntitiesServer(Registry& registry);
void initializeGameServerSystems(Registry& registry, sf::RenderWindow& window,  AssetManager& assetManager,  float& elapsed_time);
void initializeDatabaseAndConfig(Config& config, Database& database, GUI& gui);
void firstLoadingScreen(GUI& gui, Database& database, Registry& registry,sf::RenderWindow& window);
void serverLoop(Registry& registry, sf::RenderWindow& window, sf::Clock& clock, float& elapsed_time, GUI& gui, Database& database);

extern std::unordered_map<std::string, ComponentAdder> componentAdderMap;
#endif /* !SERVERMAIN_HPP_ */