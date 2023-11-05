/*
** EPITECH PROJECT, 2024
** Rtype
** File description:
** main.cpp
*/

/**
** main.cpp
**
** This file contains the main function for both server and client modes of the R-Type game.
** It initializes the game's registry, configuration, GUI, database, and runs the main game loop.
**
** Functions:
**
** - gameLoop(Registry &registry, sf::RenderWindow &window, sf::Clock &clock, float &elapsed_time)
**   This function is the main game loop that clears the window, updates elapsed time, runs
**   all registered systems, displays the window content, cleans up, and manages entity creations and deletions.
**
** - MainLoop(Registry &registry, sf::RenderWindow &window, sf::Clock &clock, float &elapsed_time, GUI &gui, Database &database)
**   This function is the outer loop that handles SFML window events, and runs the game loop
**   based on the game state. It also profiles the game loop in client mode.
**
** - main()
**   The main function initializes the registry, configuration, database, GUI, and runs the MainLoop function.
**
** Dependencies:
**
** - <SFML/Graphics.hpp> for rendering.
** - <iostream> for standard I/O.
** - "Registry.hpp" for the Registry class definition.
** - "Component.hpp" for component definitions.
** - "SparseArray.hpp" for managing sparse arrays.
** - "Entity.hpp" for managing entities.
** - "Spawner.hpp" for spawning entities.
** - "TextureManager.hpp" for managing textures.
** - "Global.hpp" for global variables.
** - "Database/Database.hpp" for database management.
** - "GUI/GUI.hpp" for GUI management.
** - "Config/Config.hpp" for configuration management.
** - "taskLoader.hpp" for loading tasks in server mode.
** - "ServerMain.hpp" for server main logic.
** - "ClientMain.hpp" for client main logic.
** - "ConfigServer.hpp" for server configuration in client mode.
**
*/

#include <SFML/Graphics.hpp>
#include <iostream>
#include "Registry.hpp"  // Assume this header contains your Registry class definition
#include "Component.hpp" // Assume this header contains your component definitions
#include "SparseArray.hpp"
#include "Entity.hpp"
#include "Spawner.hpp"
#include "TextureManager.hpp"
#include "Global.hpp"
#include  "Database/Database.hpp"
#include "GUI/GUI.hpp"
#include "Config/Config.hpp"

#ifdef SERVER_MODE
#include "taskLoader.hpp"
#include "ServerMain.hpp"
#include "ConfigServer.hpp"
#endif
#ifdef CLIENT_MODE
#include "ClientMain.hpp"
#include "ConfigServer.hpp"
#endif

const float FIXED_TIME_STEP = 1.0f / 60.0f;  // 60 updates per second. 1/60 = 0.01666666666

void gameLoop(Registry &registry, sf::RenderWindow &window, float &accumulator, float &frameTime, float &deltaTime) {
    // Run fixed updates as many times as necessary to "catch up"
    while (accumulator >= FIXED_TIME_STEP) {

        registry.run_systems(); // Run logic updates with fixed timestep
        cleanup_system(registry);
        registry.delete_queued_entities();
        registry.create_queued_entities();
        registry.AddComponentsToNewEntities();
        deltaTime = FIXED_TIME_STEP;
        accumulator -= FIXED_TIME_STEP; // Decrease accumulator by the fixed timestep
    }

    registry.run_draw_system(); // Run rendering every frame
    window.display(); // Swap buffers and display the current frame
}

void MainLoop(Registry &registry, sf::RenderWindow &window, sf::Clock &clock, float &elapsed_time, GUI &gui,
                Database &database) {
    std::vector<std::function<void(int &)>> tasks;
    float accumulator = 0.0f;  // Accumulator for fixed timestep logic updates

#ifdef SERVER_MODE
    spawnServerEntity(registry);

    while (window.isOpen()) {
        // Handle SFML events
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
        }
                        float frameTime = clock.restart().asSeconds(); // Time since last frame
                accumulator += frameTime; // Accumulate the frame time

                gameLoop(registry, window, accumulator, frameTime, elapsed_time);
    }
#endif
#ifdef CLIENT_MODE
    registry.stateEntity = spawnState(registry);
    auto &state = registry.get_components<State>()[registry.stateEntity];
    if (!state.has_value())
        return;
    while (window.isOpen()) {
        // Handle SFML events
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
        }
        window.clear();
        switch (state->state) {
            case GameState::EXIT:
                window.close();
                break;
            default:
                float frameTime = clock.restart().asSeconds(); // Time since last frame
                accumulator += frameTime; // Accumulate the frame time
                gameLoop(registry, window, accumulator, frameTime, elapsed_time);
                break;
        }

    }
#endif
}

int main() {
    Registry registry;
    Config config;

    Database database;
    sf::RenderWindow window;
#ifdef SERVER_MODE
    window.create(sf::VideoMode(1920, 1080), "R-Type Server");
#endif

#ifdef CLIENT_MODE
    window.create(sf::VideoMode(1920, 1080), "R-Type Client");
#endif

    window.setFramerateLimit(FRAMERATE);
    GUI gui(window);
    AssetManager& assetManager = AssetManager::Instance();
    loadAllTexture(assetManager);

    // Timing variables for frame animation
    sf::Clock clock = sf::Clock();
    float elapsed_time = 0.0f;

    register_all_components(registry);

#ifdef SERVER_MODE
    loadConfig("config/ClientConnectionConfig.cfg");
    initializeGameServerSystems(registry, window, assetManager, elapsed_time);
    initializeDatabaseAndConfig(config, database, gui);
   // firstLoadingScreen(gui, database, registry, window);
#endif

#ifdef CLIENT_MODE
    // Add client initialization and loop here
    loadConfig("config/ClientConnectionConfig.cfg");
    registerClientSystem(registry, window, assetManager, elapsed_time);
#endif
    MainLoop(registry, window, clock, elapsed_time, gui, database);
    return 0;
}

