/*
** EPITECH PROJECT, 2024
** Rtype
** File description:
** ServerMain.cpp
*/

/**
** ServerMain.cpp
**
** This file houses the primary functions required for initializing and running the server-side
** game systems. It encapsulates the setup for the game server systems, database, configurations,
** and the initial loading screen.
**
** Functions:
**
** - initializeGameServerSystems(Registry &registry, sf::RenderWindow &window, AssetManager &assetManager, float &elapsed_time)
**   This function encapsulates the process of initializing the game server systems by registering
**   server systems with the game's Registry, window, asset manager, and tracking the elapsed time.
**
** - initializeDatabaseAndConfig(Config &config, Database &database, GUI &gui)
**   This function encapsulates the process of initializing the game's configuration and database
**   by loading configurations from files and validating the database contents.
**
** - firstLoadingScreen(GUI &gui, Database &database, Registry &registry, sf::RenderWindow &window)
**   This function encapsulates the initial loading screen setup, including creating the loading screen,
**   setting up loading tasks, executing these tasks, and transitioning to the main game loop.
**
** Dependencies:
**
** - "ServerMain.hpp" for the declarations of the initialization functions.
** - "SceneParsing.hpp" for parsing scene configurations.
** - <iostream> for standard I/O operations.
** - <vector> for std::vector.
** - <functional> for std::function.
** - <SFML/Graphics.hpp> for rendering and window management.
**
*/

#include "ServerMain.hpp"
#include "SceneParsing.hpp"

void initializeGameServerSystems(Registry &registry, sf::RenderWindow &window, AssetManager &assetManager,
                                 float &elapsed_time) {
    // Register components
    registerServerSystem(registry, window, assetManager, elapsed_time);
}



void initializeDatabaseAndConfig(Config &config, Database &database, GUI &gui) {
    database.load("./config/Database.cfg");
    for (int i = 0; i < database.getNbBeatmaps(); ++i) {
        database.isBeatmapExist(i);
    }
    database.print();
    config.load("config/game.cfg");
    config.print();
}

void firstLoadingScreen(GUI &gui, Database &database, Registry &registry, sf::RenderWindow &window) {

    gui.loadingScreenCreate();
    gui.setActive(false);
    std::vector<std::function<void(int &)>> tasks;

    tasks.push_back([&](int &progress) {
        gui.menu.load(make_pair(gui.getWindow().getSize().x, gui.getWindow().getSize().y), database);
    });


    // Call the function to execute tasks with the loading screen
    executeLoadingTasks(gui, tasks);

    window.clear();


    // Main game loop
}

