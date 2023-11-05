/*
** EPITECH PROJECT, 2024
** Rtype
** File description:
** SceneParsing
*/

/**
** SceneParsing
**
** This file contains functions to parse scene config files and initialize entities.
**
** Functions:
**
** - addPosition()
**   Adds Position component to entity based on config data.
**
** - addVelocity()
**   Adds Velocity component to entity based on config data.
**
** - addCollision()
**   Adds Collision component to entity based on config data.
**
** - addDrawable()
**   Adds Drawable component to entity based on config data.
**
** - addAcceleration()
**   Adds Acceleration component to entity based on config data.
**
** - addAnimation()
**   Adds Animation component to entity based on config data.
**
** - addGameConfig()
**   Adds GameConfig component to entity based on config data.
**
** - addControllable()
**   Adds Controllable component to entity based on config data.
**
** - addLaser()
**   Adds Laser component to entity based on config data.
**
** - addDepth()
**   Adds Depth component to entity based on config data.
**
** - addBehavior()
**   Adds Behavior component to entity based on config data.
**
** - addFFT()
**   Adds FFT component to entity based on config data.
**
** - addSpawner()
**   Adds Spawner component to entity based on config data.
**
** - addFollower()
**   Adds Follower component to entity based on config data.
**
** - addDeathEffect()
**   Adds DeathEffect component to entity based on config data.
**
** - addParent()
**   Adds Parent component to entity based on config data.
**
** - initializeGameEntitiesFromConfig()
**   Initializes game entities based on a config file.
*/

#ifndef SCENEPARSING_HPP_
#define SCENEPARSING_HPP_

#include <libconfig.h++>
#include "Component.hpp"
#include "System/EffectsFunctions.hpp"

void addPosition(entity_t entity, Registry& registry, const libconfig::Setting& componentData);
void addVelocity(entity_t entity, Registry& registry, const libconfig::Setting& componentData);
void addCollision(entity_t entity, Registry& registry, const libconfig::Setting& componentData);
void addDrawable(entity_t entity, Registry& registry, const libconfig::Setting& componentData);
void addAcceleration(entity_t entity, Registry& registry, const libconfig::Setting& componentData);
void addAnimation(entity_t entity, Registry& registry, const libconfig::Setting& componentData);
void addGameConfig(entity_t entity, Registry& registry, const libconfig::Setting& componentData);
void addControllable(entity_t entity, Registry& registry, const libconfig::Setting& componentData);
void addLaser(entity_t entity, Registry& registry, const libconfig::Setting& componentData);
void addDepth(entity_t entity, Registry& registry, const libconfig::Setting& componentData);
void addBehavior(entity_t entity, Registry& registry, const libconfig::Setting& componentData);
void addFFT(entity_t entity, Registry& registry, const libconfig::Setting& componentData);
void addSpawner(entity_t entity, Registry& registry, const libconfig::Setting& componentData);
void addFollower(entity_t entity, Registry& registry, const libconfig::Setting& componentData);
void addDeathEffect(entity_t entity, Registry& registry, const libconfig::Setting& componentData);
void addParent(entity_t entity, Registry& registry, const libconfig::Setting& componentData);
void initializeGameEntitiesFromConfig(Registry &registry, const std::string &configFilePath);
#endif /* !SCENEPARSING_HPP_ */