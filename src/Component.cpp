/*
** EPITECH PROJECT, 2023
** Rtype
** File description:
** Component.cpp
*/

/**
** Component.cpp
**
** Implements various utility functions, system registration functions,
** and component registration for the game's entity component system (ECS).
**
** Functions:
**
** - registerServerSystem(Registry &registry, sf::RenderWindow &window, AssetManager &textureManager, float &elapsed_time)
**   Registers server systems to the registry, with the necessary dependencies injected.
**
** - registerClientSystem(Registry &registry, sf::RenderWindow &window, AssetManager &textureManager, float &elapsed_time)
**   Registers client systems to the registry, with the necessary dependencies injected.
**
** - cleanup_system(Registry &registry)
**   Cleans up entities that have moved out of the screen bounds,
**   or meet other conditions for cleanup.
**
** - register_all_components(Registry &registry)
**   Registers all the components used in the game to the registry.
**
** Global Variables:
**
** - colliderTypeMap, entityTypeMap, deathApplyTypeMap
**   These maps map string names to enum values for different types.
**
** Dependencies:
**
** - Various headers for the ECS framework, systems, and components.
** - SFML for rendering and other game-related functionalities.
**
** Notes:
**
** - Each system registration function and cleanup function could
**   be part of a larger game initialization and management module or class.
**
*/

#include "Component.hpp"
#include "Registry.hpp"
#include "TextureManager.hpp"
#include "../include/System/System.hpp"
#include "../include/System/AnimationSystem.hpp"
#include "../include/System/BehaviourSystem.hpp"
#include "../include/System/Quadtree.hpp"
#include "../include/System/ServerSystem.hpp"
#include "../include/System/DrawSystem.hpp"
#include "../include/System/ControlSystem.hpp"
#include "../include/System/FFTSystem.hpp"
#include "../include/System/VelocitySystem.hpp"
#include "../include/System/ClickableSystem.hpp"
#include "../include/System/SpawnSystem.hpp"
#include "../include/System/FollowerSystem.hpp"
#include "../include/System/MenuSystem.hpp"
#include "../include/System/ClientSystem.hpp"
#include "../include/System/DeathEffectSystem.hpp"

std::unordered_map<std::string, ColliderType> colliderTypeMap = {
        {"STOP",     ColliderType::STOP},
        {"BOUNCING", ColliderType::BOUNCING},
        {"DESTROY",  ColliderType::DESTROY}
};

std::unordered_map<std::string, EntityType> entityTypeMap = {
        {"ENEMY",     EntityType::ENEMY},
        {"FRIENDLY",  EntityType::FRIENDLY},
        {"FRIENDLY_HOST",  EntityType::FRIENDLY_HOST},
        {"FRIENDLY_BULLET",  EntityType::FRIENDLY_BULLET},
        {"FRIENDLY_PROJECTILE_HOST",  EntityType::FRIENDLY_PROJECTILE_HOST},
        {"OBSTACLE",  EntityType::OBSTACLE}
};

std::unordered_map<std::string, DeathApplyType> deathApplyTypeMap = {
        {"TO_HOST",     DeathApplyType::TO_HOST},
        {"TO_CLIENT",  DeathApplyType::TO_CLIENT},
        {"TO_ALL",  DeathApplyType::TO_ALL},
        {"TO_PARENT",  DeathApplyType::TO_PARENT}
};

void registerServerSystem(Registry &registry, sf::RenderWindow &window, AssetManager &textureManager, float &elapsed_time) {
    registry.add_system<ServerSystem, Network>(std::make_unique<ServerSystem>(registry, elapsed_time));;
}

void registerClientSystem(Registry &registry, sf::RenderWindow &window, AssetManager &textureManager, float &elapsed_time) {
    registry.add_system<DrawSystem, Drawable, Position, Depth, TextDrawable>(std::make_unique<DrawSystem>(window, textureManager, registry, elapsed_time));
    registry.add_system<MenuSystem, State>(std::make_unique<MenuSystem>(registry, textureManager, elapsed_time));
    registry.add_system<ClientSystem, State>(std::make_unique<ClientSystem>(registry, elapsed_time));
    registry.add_system<ClickSystem, isClickable, Position>(std::make_unique<ClickSystem>(window, registry, elapsed_time));
    registry.add_system<FFTSystem, FFT>(std::make_unique<FFTSystem>(elapsed_time, registry));
    registry.add_system<AnimationSystem, Animation, Drawable>(std::make_unique<AnimationSystem>(elapsed_time));
    registry.add_system<ControlSystem, Position, Velocity, Controllable>(std::make_unique<ControlSystem>(elapsed_time, registry));
    registry.add_system<CollisionSystem, Position, Drawable, Collision>(std::make_unique<CollisionSystem>(registry));
    registry.add_system<VelocitySystem, Position, Velocity>(std::make_unique<VelocitySystem>(elapsed_time, registry));
    registry.add_system<SpawnSystem, Spawner, Position>(std::make_unique<SpawnSystem>(elapsed_time, textureManager, registry));
    registry.add_system<BehaviorSystem, Behavior, Position>(std::make_unique<BehaviorSystem>(elapsed_time));
    registry.add_system<FollowerSystem, Follower, Position>(std::make_unique<FollowerSystem>());
    registry.add_system<DeathEffectSystem, DeathEffect, Position>(std::make_unique<DeathEffectSystem>(registry));
}

void cleanup_system(Registry &registry) {
    // Define screen dimensions
    auto &positionComponents = registry.get_components<Position>();
    auto &laserComponents = registry.get_components<Laser>();
    auto &enemyComponents = registry.get_components<Enemy>();
    auto &gameConfigComponents = registry.get_components<GameConfig>();
    GameConfig config{1920, 1080, 100};
    if (gameConfigComponents.empty()) {
    } else {
        auto &config = *gameConfigComponents[0];
    }
    int margin = 100;  // Additional pixels to keep entities in memory

    // Note: Assuming that all entities have a Position component.
    for (entity_t entity = 0; entity < positionComponents.size(); ++entity) {
        // Check if the entity has a position
        if (auto posOpt = positionComponents[entity]) {
            // Check if the entity is out of screen bounds with margin
            if (posOpt->x > config.screenWidth + margin || posOpt->y > config.screenHeight + margin ||
                posOpt->x < +margin || posOpt->y < -margin) {

                // For lasers
                if (laserComponents.contains(entity)) {
                    registry.queue_for_deletion(entity);
                }
                // For enemies or any other entity type
                if (enemyComponents.contains(entity)) {
                    registry.queue_for_deletion(entity);
                }
                // Add more conditions for other types of entities here
            }
        }
    }
}


void register_all_components(Registry &registry) {
    registry.register_component<Position>();
    registry.register_component<Velocity>();
    registry.register_component<Drawable>();
    registry.register_component<Controllable>();
    registry.register_component<Acceleration>();
    registry.register_component<Spawner>();
    registry.register_component<Laser>();
    registry.register_component<Depth>();
    registry.register_component<Behavior>();
    registry.register_component<Enemy>();
    registry.register_component<GameConfig>();
    registry.register_component<Health>();
    registry.register_component<FFT>();
    registry.register_component<Animation>();
    registry.register_component<Follower>();
    registry.register_component<Collision>();
    registry.register_component<Network>();
    registry.register_component<State>();
    registry.register_component<Client>();
    registry.register_component<UserInput>();
    registry.register_component<isClickable>();
    registry.register_component<TextDrawable>();
    registry.register_component<DeathEffect>();
    registry.register_component<IsChild>();
    registry.register_component<idSync>();
}

