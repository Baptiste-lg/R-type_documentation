/*
** EPITECH PROJECT, 2024
** Rtype
** File description:
** ENTITY
*/

/**
 * Entity.hpp defines the Entity class used in the ECS.
 *
 * An Entity represents a game object / actor.
 * It is simply a numeric ID used as a key in component arrays.
 *
 * Entities:
 * - Represent game objects (player, enemy, projectile etc).
 * - Are numeric IDs associated with components.
 *
 * Usage:
 * - Entity IDs are returned when spawning entities.
 * - Pass Entity ID to component arrays to access that entity.
 * - Entity IDs can be implicitly cast to size_t.
 *
*/

#ifndef ENTITY_HPP_
#define ENTITY_HPP_

#include <cstddef> // for size_t
using entity_t = std::size_t;
class Entity {
public:
    explicit Entity(std::size_t id) : id_(id) {}

    // Implicit conversion to size_t
    operator std::size_t() const {
        return id_;
    }

private:
    std::size_t id_;
};

#endif /* !ENTITY_HPP_ */
