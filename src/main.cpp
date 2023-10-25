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
#endif
#ifdef CLIENT_MODE
#include "ClientMain.hpp"
#include "ConfigServer.hpp"
#endif




int main() {
    Registry registry;
    Config config;

    Database database;

#ifdef SERVER_MODE
    sf::RenderWindow window(sf::VideoMode(1920 , 1080), "ECS Example");
    window.setFramerateLimit(FRAMERATE);
    GUI gui(window);
#endif

    AssetManager& assetManager = AssetManager::Instance();
    loadAllTexture(assetManager);

    // Timing variables for frame animation
    sf::Clock clock = sf::Clock();
    float elapsed_time = 0.0f;

    register_all_components(registry);

#ifdef SERVER_MODE
    initializeGameServerSystems(registry, window, assetManager, elapsed_time);

    initializeDatabaseAndConfig(config, database, gui);
    firstLoadingScreen(gui, database, registry, window);
    serverLoop(registry, window, clock, elapsed_time, gui, database);
#endif

#ifdef CLIENT_MODE
    std::cout << "SO COOL TO BE A CLIENT" << std::endl;
    // Add client initialization and loop here
    loadConfig("config/ClientConnectionConfig.cfg");
    initializeGameEntitiesClient(registry);
    registerClientSystem(registry, elapsed_time);
    while (true) {
        registry.run_systems();  // Run all systems, including ClientSystem
    }

#endif

    return 0;
}

