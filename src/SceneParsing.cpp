/*
** EPITECH PROJECT, 2024
** Rtype
** File description:
** SceneParsing.cpp
*/

/**
** SceneParsing.cpp
**
** This file includes functionality to parse and translate configurations from a
** file into usable in-game components. It utilizes the libconfig library to read
** configurations and a set of defined component adding functions to populate
** the game's Registry with corresponding entities and components.
**
** Variables:
**
** - componentAdderMap
**   A global unordered map associating strings representing component types
**   to functions that know how to add these components to entities.
**
** Functions:
**
** - getComponentAdderByName(const std::string& name)
**   This function returns a pointer to a ComponentAdder function associated
**   with a given component name, or null if no such function is found.
**
** - makeAdder(entity_t parent, const std::string& name, const libconfig::Setting& params)
**   A factory function to create ComponentSpawner functions for a specified component
**   type and parameters, for a specified parent entity.
**
** - addPosition(entity_t entity, Registry& registry, const libconfig::Setting& componentData)
**   Adds a Position component to a specified entity in the registry using provided data.
**
** - addVelocity(entity_t entity, Registry& registry, const libconfig::Setting& componentData)
**   Adds a Velocity component to a specified entity in the registry using provided data.
**
** - addCollision(entity_t entity, Registry& registry, const libconfig::Setting& componentData)
**   Adds a Collision component to a specified entity in the registry using provided data.
**
** - addDrawable(entity_t entity, Registry& registry, const libconfig::Setting& componentData)
**   Adds a Drawable component to a specified entity in the registry using provided data.
**
** - addAcceleration(entity_t entity, Registry& registry, const libconfig::Setting& componentData)
**   Adds an Acceleration component to a specified entity in the registry using provided data.
**
** - addAnimation(entity_t entity, Registry& registry, const libconfig::Setting& componentData)
**   Adds an Animation component to a specified entity in the registry using provided data.
**
** - addGameConfig(entity_t entity, Registry& registry, const libconfig::Setting& componentData)
**   Adds a GameConfig component to a specified entity in the registry using provided data.
**
** - addControllable(entity_t entity, Registry& registry, const libconfig::Setting& componentData)
**   Adds a Controllable component to a specified entity in the registry using provided data.
**
** - addLaser(entity_t entity, Registry& registry, const libconfig::Setting& componentData)
**   Adds a Laser component to a specified entity in the registry using provided data.
**
** - addDepth(entity_t entity, Registry& registry, const libconfig::Setting& componentData)
**   Adds a Depth component to a specified entity in the registry using provided data.
**
** - addBehavior(entity_t entity, Registry& registry, const libconfig::Setting& componentData)
**   Adds a Behavior component to a specified entity in the registry using provided data.
**
** - addFFT(entity_t entity, Registry& registry, const libconfig::Setting& componentData)
**   Adds a FFT component to a specified entity in the registry using provided data.
**
** - addSpawner(entity_t entity, Registry& registry, const libconfig::Setting& componentData)
**   Adds a Spawner component to a specified entity in the registry using provided data.
**
** - addFollower(entity_t parentEntity, Registry& registry, const libconfig::Setting& componentData)
**   Adds a Follower component to a specified entity in the registry using provided data.
**
** - addDeathEffect(entity_t entity, Registry& registry, const libconfig::Setting& componentData)
**   Adds a DeathEffect component to a specified entity in the registry using provided data.
**
** - initializeGameEntitiesFromConfig(Registry &registry, const std::string &configFilePath)
**   Reads a configuration file to populate the Registry with entities and components
**   as specified in the configuration.
**
** Dependencies:
**
** - "SceneParsing.hpp" for the declarations of parsing functions and component adders.
** - "Global.hpp" for global game settings and typedefs.
** - <unordered_map> for std::unordered_map.
** - <iostream> for standard I/O operations.
** - <libconfig.h++> for reading configuration files.
** - <vector> for std::vector.
** - <string> for std::string.
**
*/

#include "SceneParsing.hpp"
#include "Global.hpp"
std::unordered_map<std::string, ComponentAdder> componentAdderMap = {
        {"Position", addPosition},
        {"Velocity", addVelocity},
        {"Drawable", addDrawable},
        {"Collision", addCollision},
        {"Acceleration", addAcceleration},
        {"Animation", addAnimation},
        {"GameConfig", addGameConfig},
        {"Controllable", addControllable},
        {"Laser", addLaser},
        {"Depth", addDepth},
        {"Behavior", addBehavior},
        {"FFT", addFFT},
        {"Spawner", addSpawner},
        {"Follower", addFollower},
        {"DeathEffect", addDeathEffect},
};

ComponentAdder* getComponentAdderByName(const std::string& name) {
    if (componentAdderMap.find(name) != componentAdderMap.end()) {
        return &componentAdderMap[name];
    }
    return nullptr;
}

ComponentSpawner makeAdder(entity_t parent, const std::string& name, const libconfig::Setting& params) {
    if (name == "addPosition") {
        float x = params["x"];
        float y = params["y"];
        return [x, y](entity_t entity, Registry &registry) {
            registry.add_component(entity, Position{x, y, x, y});
        };
    }
    // Similar functions for other component types
    if (name == "addVelocity") {
        float x = params["x"];
        float y = params["y"];
        return [x, y](entity_t entity, Registry &registry) {
            registry.add_component(entity, Velocity{x, y});
        };
    }
    if (name == "addAcceleration") {
        float x = params["x"];
        float y = params["y"];
        return [x, y](entity_t entity, Registry &registry) {
            registry.add_component(entity, Acceleration{x, y});
        };
    }
    if (name == "addDrawable") {
        std::string textureName = params["textureName"];
        MyRect textureRect;
        textureRect.left = params["textureRect"][0];
        textureRect.top = params["textureRect"][1];
        textureRect.width = params["textureRect"][2];
        textureRect.height = params["textureRect"][3];
        return [textureName, textureRect](entity_t entity, Registry &registry) {
            registry.add_component(entity, Drawable{textureName, textureRect});
        };
    }
    if (name == "addCollision") {
        std::string entityType = params["entityType"];
        std::string colliderType = params["colliderType"];
        auto it = colliderTypeMap.find(colliderType);
        if (it != colliderTypeMap.end()) {
            auto collider = it->second;
            auto it2 = entityTypeMap.find(entityType);
            if (it2 != entityTypeMap.end()) {
                auto type = it2->second;
                return [type, collider](entity_t entity, Registry &registry) {
                    registry.add_component(entity, Collision{type, collider});
                };
            } else {
                std::cerr << "No entity type found for name: " << entityType << std::endl;
                return nullptr;  // Keep the return statement here to exit early in case of an error
            }
        } else {
            std::cerr << "No collider type found for name: " << colliderType << std::endl;
            return nullptr;  // Keep the return statement here to exit early in case of an error
        }
    }
    if (name == "addAnimation") {
        int currentFrame = params["currentFrame"];
        int numFrames = params["numFrames"];
        float frameDuration = params["frameDuration"];
        float elapsedTime = params["elapsedTime"];
        bool loop = params["loop"];
        return [currentFrame, numFrames, frameDuration, elapsedTime, loop](entity_t entity, Registry &registry) {
            registry.add_component(entity, Animation{currentFrame, numFrames, frameDuration / FRAMERATE, elapsedTime, loop});
        };
    }
    if (name == "addGameConfig") {
        float screenWidth = params["screenWidth"];
        float screenHeight = params["screenHeight"];
        float playerSpeed = params["playerSpeed"];
        return [screenWidth, screenHeight, playerSpeed](entity_t entity, Registry &registry) {
            registry.add_component(entity, GameConfig{screenWidth, screenHeight, playerSpeed});
        };
    }
    if (name == "addControllable") {
        bool isLocal = params["isLocal"];
        int client_id = params["client_id"];
        return [isLocal, client_id](entity_t entity, Registry &registry) {
            registry.add_component(entity, Controllable{isLocal, client_id});
        };
    }
    if (name == "addLaser") {
        return [](entity_t entity, Registry &registry) {
            registry.add_component(entity, Laser{});
        };
    }
    if (name == "addDepth") {
        float z = params["z"];
        return [z](entity_t entity, Registry &registry) {
            registry.add_component(entity, Depth{z});
        };
    }
    if (name == "addBehavior") {
        std::string behaviorID = params["type"];
        return [behaviorID](entity_t entity, Registry &registry) {
            registry.add_component(entity, Behavior{behaviorID});
        };
    }
    if (name == "addFFT") {
        return [](entity_t entity, Registry &registry) {
            registry.add_component(entity, FFT{});
        };
    }
if (name == "addDeathEffect") {
    std::string effectName = params["type"];
    std::string applyTo = params["applyTo"];
    auto it = EffectFunctionMap.find(effectName);
    if (it != EffectFunctionMap.end()) {
        auto effect = it->second;  // Capture the effect function
        auto it2 = deathApplyTypeMap.find(applyTo);
        if (it2 != deathApplyTypeMap.end()) {
            auto type = it2->second;  // Capture the entity type
            return [effectName, effect, type](entity_t entity, Registry &registry) {
                registry.add_component(entity, DeathEffect{effectName, effect, type});
            };
        } else {
            std::cerr << "No entity type found for name: " << applyTo << std::endl;
        }
    } else {
        std::cerr << "No death effect found for name: " << effectName << std::endl;
    }
}

    if (name == "addParent") {
        return [parent](entity_t entity, Registry &registry) {
            registry.add_component(entity, IsChild{parent});
        };
    }
    return nullptr;
}

void addPosition(entity_t entity, Registry& registry, const libconfig::Setting& componentData) {
    float x, y;
    componentData.lookupValue("x", x);
    componentData.lookupValue("y", y);
    registry.add_component(entity, Position{x, y, x, y});
}

// Similar functions for other component types

void addVelocity(entity_t entity, Registry& registry, const libconfig::Setting& componentData) {
    float x, y;
    componentData.lookupValue("x", x);
    componentData.lookupValue("y", y);
    registry.add_component(entity, Velocity{x, y});
}

void addCollision(entity_t entity, Registry& registry, const libconfig::Setting& componentData) {
    std::string entityType;
    std::string colliderType ;
    componentData.lookupValue("entityType", entityType);
    componentData.lookupValue("colliderType", colliderType);
    auto it = colliderTypeMap.find(colliderType);
    if (it != colliderTypeMap.end()) {
        auto collider = it->second;
        auto it2 = entityTypeMap.find(entityType);
        if (it2 != entityTypeMap.end()) {
            auto type = it2->second;
            registry.add_component(entity, Collision{type, collider});
        } else {
            std::cerr << "No entity type found for name: " << entityType << std::endl;
            return;  // Keep the return statement here to exit early in case of an error
        }
    } else {
        std::cerr << "No collider type found for name: " << colliderType << std::endl;
        return;  // Keep the return statement here to exit early in case of an error
    }
}


void addDrawable(entity_t entity, Registry& registry, const libconfig::Setting& componentData) {
    std::string textureName;
    MyRect textureRect;
    componentData.lookupValue("textureName", textureName);
    const libconfig::Setting& textureRectSetting = componentData["textureRect"]; // Fixed this line
    textureRect.left = textureRectSetting[0];
    textureRect.top = textureRectSetting[1];
    textureRect.width = textureRectSetting[2];
    textureRect.height = textureRectSetting[3];
    registry.add_component(entity, Drawable{textureName, textureRect});
}

void addAcceleration(entity_t entity, Registry& registry, const libconfig::Setting& componentData) {
    float x, y;
    componentData.lookupValue("x", x);
    componentData.lookupValue("y", y);
    registry.add_component(entity, Acceleration{x, y});
}

void addAnimation(entity_t entity, Registry& registry, const libconfig::Setting& componentData) {
    int currentFrame = 0;
    int numFrames = 1;
    float frameDuration = 0.0f;
    float elapsedTime = 0.0f;
    bool loop = true;

    componentData.lookupValue("currentFrame", currentFrame);
    componentData.lookupValue("numFrames", numFrames);
    componentData.lookupValue("frameDuration", frameDuration);
    componentData.lookupValue("elapsedTime", elapsedTime);
    componentData.lookupValue("loop", loop);
    registry.add_component(entity, Animation{currentFrame, numFrames, frameDuration/FRAMERATE, elapsedTime, loop});
}

void addGameConfig(entity_t entity, Registry& registry, const libconfig::Setting& componentData) {
    float screenWidth, screenHeight, playerSpeed;
    componentData.lookupValue("screenWidth", screenWidth);
    componentData.lookupValue("screenHeight", screenHeight);
    componentData.lookupValue("playerSpeed", playerSpeed);
    registry.add_component(entity, GameConfig{screenWidth, screenHeight, playerSpeed});
}

void addControllable(entity_t entity, Registry& registry, const libconfig::Setting& componentData) {
    bool isLocal = false;
    int client_id = 0;
    componentData.lookupValue("isLocal", isLocal);
    componentData.lookupValue("client_id", client_id);
    registry.add_component(entity, Controllable{isLocal, client_id});

}

void addLaser(entity_t entity, Registry& registry, const libconfig::Setting& componentData) {
    registry.add_component(entity, Laser{});
}

void addDepth(entity_t entity, Registry& registry, const libconfig::Setting& componentData) {
    float z = 0.0f;
    componentData.lookupValue("z", z);
    registry.add_component(entity, Depth{z});
}

void addBehavior(entity_t entity, Registry& registry, const libconfig::Setting& componentData) {
    std::string behaviorID;
    Behavior behavior;
    componentData.lookupValue("type", behaviorID);
    registry.add_component(entity, Behavior{behaviorID});
}


void addFFT(entity_t entity, Registry& registry, const libconfig::Setting& componentData) {
    registry.add_component(entity, FFT{});
}
void addSpawner(entity_t entity, Registry& registry, const libconfig::Setting& componentData) {
    float spawnRate = 0.0f;
    float timeSinceLastSpawn = 0.0f;
    std::string entityToSpawn;
    float xOffset = 0.0f;
    float yOffset = 0.0f;
    float xVelocity = 0.0f;
    float yVelocity = 0.0f;
    std::vector<ComponentSpawner > componentAdders;

    componentData.lookupValue("spawnRate", spawnRate);
    componentData.lookupValue("timeSinceLastSpawn", timeSinceLastSpawn);
    componentData.lookupValue("entityToSpawn", entityToSpawn);
    componentData.lookupValue("xOffset", xOffset);
    componentData.lookupValue("yOffset", yOffset);
    componentData.lookupValue("xVelocity", xVelocity);
    componentData.lookupValue("yVelocity", yVelocity);
    // Check if componentAdders is present
// Inside your function that populates the Spawner component
    if (componentData.exists("componentAdders")) {
        const libconfig::Setting& componentAddersData = componentData["componentAdders"];
        for (int i = 0; i < componentAddersData.getLength(); ++i) {
            const libconfig::Setting& componentAdderData = componentAddersData[i];
            std::string name;
            componentAdderData.lookupValue("name", name);
            const libconfig::Setting& params = componentAdderData["params"];

            ComponentSpawner adder = makeAdder(entity, name, params);
            if (adder) {
                componentAdders.push_back(adder);
            }
        }
    }
    registry.add_component(entity, Spawner{spawnRate, timeSinceLastSpawn, entityToSpawn, xOffset, yOffset, xVelocity, yVelocity, componentAdders});
}

void addFollower(entity_t parentEntity, Registry& registry, const libconfig::Setting& componentData) {
    // Read the basic settings
    float xOffset = 0.0f;
    float yOffset = 0.0f;
    std::string type;

    componentData.lookupValue("xOffset", xOffset);
    componentData.lookupValue("yOffset", yOffset);
    componentData.lookupValue("type", type); // Assuming 'type' is the entity type of the follower

    // Create the follower entity
    entity_t followerEntity = registry.spawn_entity();

    // Add the Follower component to it
    registry.add_component(followerEntity, Follower{parentEntity, xOffset, yOffset});

    // Add other components based on the componentAdders
    if (componentData.exists("componentAdders")) {
        const libconfig::Setting& componentAddersData = componentData["componentAdders"];
        for (int i = 0; i < componentAddersData.getLength(); ++i) {
            const libconfig::Setting& componentAdderData = componentAddersData[i];
            std::string name;
            componentAdderData.lookupValue("name", name);

            const libconfig::Setting& params = componentAdderData["params"];
            ComponentSpawner adder = makeAdder(parentEntity, name, params); // Assuming makeAdder returns a ComponentSpawner
            if (adder)
                adder(followerEntity, registry);
        }
        //add the position in a kinda hacky way
    }
}

void addDeathEffect(entity_t entity, Registry& registry, const libconfig::Setting& componentData) {
    std::string effectName;
    std::string applyTo;
    componentData.lookupValue("type", effectName);
    componentData.lookupValue("applyTo", applyTo);
    auto it = EffectFunctionMap.find(effectName);
    if (it != EffectFunctionMap.end()) {
        auto effect = it->second;
        auto it2 = deathApplyTypeMap.find(applyTo);
        if (it2 != deathApplyTypeMap.end()) {
            auto type = it2->second;
            registry.add_component(entity, DeathEffect{effectName, effect, type});
        } else {
            std::cerr << "No entity type found for name: " << applyTo << std::endl;
            return;  // Keep the return statement here to exit early in case of an error
        }
    } else {
        std::cerr << "No death effect found for name: " << effectName << std::endl;
        return;  // Keep the return statement here to exit early in case of an error
    }
}

void initializeGameEntitiesFromConfig(Registry &registry, const std::string &configFilePath) {
    libconfig::Config cfg;
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
    registry.clientToEntityMap.clear();
    const libconfig::Setting &root = cfg.getRoot();
    const libconfig::Setting &entities = root["entities"];
    bool foundPlayer = false;
    for (int i = 0; i < entities.getLength(); ++i) {
        const libconfig::Setting &entityData = entities[i];
        auto entity = registry.spawn_entity();

        // Iterate over all fields in the entity's configuration
        for (int j = 0; j < entityData.getLength(); ++j) {
            const libconfig::Setting &componentData = entityData[j];
            const std::string &componentName = componentData.getName();
            // we will make a little cheat here to get the entity id if a player is created:
            // there is a useless field name type="Player" in the config file associated with the player creation
            // so we will use it to get the id of the player and add it to registry
            if (componentName == "type") {
                if (foundPlayer) {
                    std::cerr << "Error: multiple players found in config file" << std::endl;
                    continue;
                }
                bool isLocal = false;
                //print the line
                componentData.lookupValue("isPlayer", isLocal);
                if (isLocal)
                    registry.clientToEntityMap[-42] = entity; // this is ugly but it works
                foundPlayer = true;
                continue;
            }
            // Find the component adder for this component
            auto it = componentAdderMap.find(componentName);
            if (it != componentAdderMap.end()) {
                // Use the component adder to add the component to the entity
                it->second(entity, registry, componentData);
            }
        }
    }
    //for debug can you add a entity that have a textDrawable component thats all
    auto entity = registry.spawn_entity();
    registry.add_component(entity, Position{0, 0});
    registry.add_component(entity, TextDrawable{"Hello World", "font", 50});
    registry.add_component(entity, Depth{0.0f});
    registry.add_component(entity, Drawable{"background", MyRect{0, 0, 1920, 1080}});

}