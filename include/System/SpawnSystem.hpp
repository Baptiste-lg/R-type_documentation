/*
** EPITECH PROJECT, 2024
** Rtype
** File description:
** SpawnSystem.hpp
*/

/**
 * SpawnSystem handles spawning new entities at a specified rate.
 *
 * It checks each entity with a Spawner component and spawns
 * new entities when enough time has elapsed.
 *
 * The new entities are positioned based on the spawner's offset.
 * Additional components are added from the spawner's component list.
 *
 * Usage:
 * - Add Spawner component to an entity to make it a spawner.
 * - Set spawnRate, x/yOffset, and componentSpawners as needed.
 *
 * Components:
 * - Spawner: Defines spawning behavior.
 * - Position: Used to offset spawned entities.
 *
 * Methods:
 * - operator(): Checks spawners and spawns entities.
 *
 * Spawning:
 * - Keeps track of time since last spawn.
 * - Spawns when time exceeds 1/spawnRate.
 * - Resets internal timer after spawning.
 * - Applies position offset and components.
*/

#ifndef SPAWNSYSTEM_HPP_
#define SPAWNSYSTEM_HPP_
#include "../include/System/ISystem.hpp"
#include "../include/System/System.hpp"
#include "../include/TextureManager.hpp"
#include "../include/Registry.hpp"



class SpawnSystem : public ISystem<Spawner, Position> {
private:
    float& deltaTime;
    AssetManager& assetManager;
    Registry& registry;

public:
    std::string const getSystemName() override {
        return "SpawnSystem";
    }
    SpawnSystem(float& deltaTime, AssetManager& assetManager, Registry& registry) : deltaTime(deltaTime), assetManager(assetManager), registry(registry) {}
    void operator()(entity_t entity, sparse_array<Spawner>& spawners, sparse_array<Position>& positions) override {

        auto spawnOpt = spawners[entity];
        auto posOpt = positions[entity];

        if (spawnOpt) {
            auto& spawn = *spawners[entity];
            float posX = 0;
            float posY = 0;
            if (posOpt) {
                auto& pos = *positions[entity];
                posX = pos.x;
                posY = pos.y;

            }

            spawn.timeSinceLastSpawn += deltaTime;
            float timeRequiredForNextSpawn = 1.0f / spawn.spawnRate;

            if (spawn.timeSinceLastSpawn >= timeRequiredForNextSpawn) {
                spawn.timeSinceLastSpawn = 0.0f;  // Reset timer
                auto projectile = registry.spawn_entity();

                registry.add_component(projectile, Position{posX + spawn.xOffset, posY + spawn.yOffset, posX + spawn.xOffset, posY + spawn.yOffset});
                for (const auto& addComponent : spawn.componentSpawners) {
                    addComponent(projectile, registry);  // Apply each ComponentSpawner to the new entity
                }

            }
        }
    }
};


#endif /* !SPAWNSYSTEM_HPP_ */