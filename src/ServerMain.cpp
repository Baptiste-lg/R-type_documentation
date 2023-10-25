

#include "ServerMain.hpp"
#include "SceneParsing.hpp"


void initializeGameEntitiesFromConfig(Registry &registry, const std::string &configFilePath) {
    libconfig::Config cfg;
    std::cout << "Loading config file: " << configFilePath << std::endl;
    try {
        cfg.readFile(configFilePath.c_str());
    } catch (const libconfig::FileIOException &fioex) {
        std::cerr << "I/O error while reading config file." << std::endl;
        return;
    } catch (const libconfig::ParseException &pex) {
        std::cerr << "Parse error at " << pex.getFile() << ":" << pex.getLine()
                  << " - " << pex.getError() << std::endl;
        return;
    }
    std::cout << "Config file loaded successfully." << std::endl;

    const libconfig::Setting &root = cfg.getRoot();
    const libconfig::Setting &entities = root["entities"];

    for (int i = 0; i < entities.getLength(); ++i) {
        const libconfig::Setting &entityData = entities[i];
        auto entity = registry.spawn_entity();

        // Iterate over all fields in the entity's configuration
        for (int j = 0; j < entityData.getLength(); ++j) {
            const libconfig::Setting &componentData = entityData[j];
            const std::string &componentName = componentData.getName();

            // Find the component adder for this component
            auto it = componentAdderMap.find(componentName);
            if (it != componentAdderMap.end()) {
                // Use the component adder to add the component to the entity
                it->second(entity, registry, componentData);
            }
            //debug print of the entity
            std::cout << "Entity " << i << " : " << componentName << std::endl;
        }
    }
}


/*
void initializeGameEntitiesServer(Registry& registry)
{
    // Initialize Drawable component with texture and animation settings
    spawnPlayer(registry, {0, 0, 176, 96});
    spawnBackgroundLayer(registry, Behavior{"Fixed"},"background", 0, 0, {0, 0, 1920, 1080}, {1.0f, 1.0f});
    spawnBackgroundLayer(registry, Behavior{"LoopDiagonalRight"},"far_planets", 0.01 * 10.0f * 60/FRAMERATE, 0.014 * 10.0f * 60/FRAMERATE, {0, 0, 272, 160}, {1.0f, 1.0f});
    spawnBackgroundLayer(registry, Behavior{"LoopDiagonalRight"},"stars", 0.02 * 10.0f * 60/FRAMERATE, 0.028 * 10.0f * 60/FRAMERATE, {0, 0, 272, 160}, {1.0f, 1.0f});
    spawnBackgroundLayer(registry, Behavior{"LoopDiagonalRight"},"big_planet", 0.03 * 10.0f * 60/FRAMERATE, 0.042 * 10.0f * 60/FRAMERATE, {0, 0, 88, 87}, {1.0f, 1.0f});
    spawnBackgroundLayer(registry, Behavior{"LoopDiagonalRight"},"ring_planet", 0.04 * 10.0f * 60/FRAMERATE, 0.056 * 10.0f * 60/FRAMERATE, {0, 0, 51, 115}, {1.0f, 1.0f});
    spawnBackgroundLayer(registry, Behavior{"LoopDiagonalRight"}, "stars", 0.05 * 10.0f * 60/FRAMERATE, 0.07 * 10.0f * 60/FRAMERATE, {0, 0, 272, 160}, {1.0f, 1.0f});
    spawnAnimatedEntity(registry, {0, 0, 126, 126}, {1.0f, 1.0f}, "green_planet", 400, 300);

    spawnEnemy(registry, Behavior{"Zigzag"}, 1920, 300, "e_ship1", {0, 0, 128, 128}, {1.0f, 1.0f});
    spawnEnemy(registry, Behavior{"MoveLeft"}, 1920, 200, "e_ship1", {0, 0, 128, 128}, {1.0f, 1.0f});
    spawnEnemy(registry, Behavior{"Circular"}, 1920, 300, "e_ship1", {0, 0, 128, 128}, {1.0f, 1.0f});
    spawnEnemy(registry, Behavior{"Diagonal"}, 1920, 400,"e_ship1", {0, 0, 128, 128}, {1.0f, 1.0f});
    spawnEnemy(registry, Behavior{"SineWave"}, 1920, 500,"e_ship1", {0, 0, 128, 128}, {1.0f, 1.0f});

    spawnEnemy(registry, Behavior{"BackAndForth"}, 500, 100, "e_ship1", {0, 0, 128, 128}, {1.0f, 1.0f});
    spawnEnemy(registry, Behavior{"LShape"}, 700, 100, "e_ship1", {0, 0, 128, 128}, {1.0f, 1.0f});
    spawnEnemy(registry, Behavior{"Random"}, 1200, 300, "e_ship1", {0, 0, 128, 128}, {1.0f, 1.0f});
}


*/
void initializeGameServerSystems(Registry &registry, sf::RenderWindow &window, AssetManager &assetManager,
                                 float &elapsed_time) {
    // Register components
    registerServerSystem(registry, window, assetManager, elapsed_time);
}

void gameLoop(Registry &registry, sf::RenderWindow &window, sf::Clock &clock, float &elapsed_time) {
    window.clear();
    elapsed_time = clock.getElapsedTime().asSeconds();
    registry.run_systems();
    clock.restart();
    window.display();
    cleanup_system(registry);
    registry.delete_queued_entities();
    registry.create_queued_entities();
    registry.AddComponentsToNewEntities();
}


void initializeDatabaseAndConfig(Config &config, Database &database, GUI &gui) {


    cout << "loading database" << endl;
    database.load("./config/Database.cfg");
    cout << "loaded database" << endl;
    for (int i = 0; i < database.getNbBeatmaps(); ++i) {
        database.isBeatmapExist(i);
    }
    cout << "checked database" << endl;
    database.print();
    cout << "printed database" << endl;

    cout << "loading config" << endl;
    config.load("config/game.cfg");
    cout << "loaded config" << endl;
    config.print();
    cout << "printed config" << endl;

    std::cout << "Loading GUI configuration" << std::endl;
}

void firstLoadingScreen(GUI &gui, Database &database, Registry &registry, sf::RenderWindow &window) {

    gui.loadingScreenCreate();
    gui.setActive(false);
    std::vector<std::function<void(int &)>> tasks;

    tasks.push_back([&](int &progress) {
        gui.menu.load(make_pair(gui.getWindow().getSize().x, gui.getWindow().getSize().y), database);
    });

    tasks.push_back([&](int &progress) {
        registry.run_fft_system();
    });

    // Call the function to execute tasks with the loading screen
    executeLoadingTasks(gui, tasks);

    std::cout << "Loaded assets" << std::endl;
    // Start the main loop
    // for the moment, the menu is the main loop

    window.clear();


    // Main game loop
}

void serverLoop(Registry &registry, sf::RenderWindow &window, sf::Clock &clock, float &elapsed_time, GUI &gui,
                Database &database) {
    std::vector<std::function<void(int &)>> tasks;
    while (window.isOpen()) {
        // Handle SFML events
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
        }
        window.clear();

        switch (std::get<2>(registry.gameStates)) {
            case GameState::MENU:
                if (gui.menu.update(event, gui.getWindow()) == 1) {
                    std::get<2>(registry.gameStates) = GameState::GAME;
                    //std::get<0>(registry.gameStates) = false;
                    std::cout << "Starting game" << std::endl;
                    registry.setPath(gui.menu.getActualSongName());
                    //clear entities that where suppsoed to be created in the old song
                    tasks.clear();

                    tasks.emplace_back([&](int &progress) {
                        initializeGameEntitiesFromConfig(registry, "Scene/GameScene.cfg");
                    });
                    tasks.emplace_back([&](int &progress) {
                        registry.run_fft_system();
                    });

                    tasks.emplace_back([&](int &progress) {
                        spawnServerEntity(registry);
                    });

                    // Call the function to execute tasks with the loading screen
                    executeLoadingTasks(gui, tasks);
                    gui.menu.setState(0);

                }
                gui.menu.draw(window);

                break;
            case GameState::GAME:
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)) {
                    std::get<2>(registry.gameStates) = GameState::MENU;
                    std::cout << "Back to menu size of constructor and inactive entities : "
                              << registry._entity_constructors.size() << " " << registry._inactive_entities.size()
                              << " " << registry._active_entities.size() << std::endl;
                    registry.clearGameState();
                    std::cout << "Back to menu size of constructor and inactive entities : "
                              << registry._entity_constructors.size() << " " << registry._inactive_entities.size()
                              << " " << registry._active_entities.size() << std::endl;

                    break;
                }
                gameLoop(registry, window, clock, elapsed_time);
                break;
            case GameState::EXIT:
                window.close();
                break;
            default:
                break;
        }
        window.display();
    }
}