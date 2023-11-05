/*
** EPITECH PROJECT, 2024
** Rtype
** File description:
** DeathEffectSystem.hpp
*/

/**
* DeathEffectSystem.hpp
*
* System to handle death effect components.
* Inherits from ISystem.
*
* Methods:
*
* - DeathEffectSystem()
*   Constructor, stores reference to registry.
*
* - operator()()
*   System update method.
*   Passed entities with DeathEffect and Position.
*
* Members:
*
* - registry
*   Reference to global ECS registry.
*
*/

#ifndef DEATHEFFECTSYSTEM_HPP_
#define DEATHEFFECTSYSTEM_HPP_

#include "ISystem.hpp"
#include "System.hpp"
#include "../Registry.hpp"
#include "Component.hpp"

class DeathEffectSystem : public ISystem<DeathEffect, Position> {
private:
    Registry& registry;
public:
    DeathEffectSystem(Registry& registry) : registry(registry) {}

    void operator()(entity_t entity, sparse_array<DeathEffect>& deathEffects, sparse_array<Position>& positions) override {

    }
    std::string const getSystemName() override {
        return "DeathEffectSystem";
    }
};

#endif /* DEATHEFFECTSYSTEM_HPP_ */
