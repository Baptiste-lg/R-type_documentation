/*
** EPITECH PROJECT, 2024
** Rtype
** File description:
** SYSTEM
*/

/**
 * VelocitySystem handles moving entities based on their Velocity component.
 *
 * Each frame, it updates an entity's Position by adding its Velocity scaled by deltaTime.
 *
 * Usage:
 * - Add Velocity component to make an entity move.
 * - Velocity is scaled by deltaTime for frame rate independence.
 *
 * Components:
 * - Position: Updated each frame based on Velocity.
 * - Velocity: Defines the movement vector.
 *
 * Methods:
 * - operator(): Applies velocity to position each frame.
 *
 * Movement:
 * - Get Velocity and Position components.
 * - Scale Velocity by deltaTime.
 * - Offset Position by scaled Velocity.
 * - Clamp Position to prevent going offscreen.
*/

#ifndef VELOCITYSYSTEM_HPP_
#define VELOCITYSYSTEM_HPP_
#include "ISystem.hpp"
#include "Component.hpp"
#include "SparseArray.hpp"





class VelocitySystem : public ISystem<Position, Velocity> {
private:
    float& deltaTime;
    std::queue<Event> heartBeatEvents;
    Registry& registry;

public:
    std::string const getSystemName() override {
        return "VelocitySystem";
    }
    VelocitySystem(float& deltaTime, Registry& registry) : deltaTime(deltaTime), registry(registry) {
        registry.eventBus.addListener(HeartBeat, [&](const Event &event) {
            // TODO: Add logic to handle HeartBeat events
            heartBeatEvents.push(event);
        });
    }
    Position interpolate(const Position& previous, const Position& current, float alpha) {
        Position interpolated;
        interpolated.x = previous.x * (1 - alpha) + current.x * alpha;
        interpolated.y = previous.y * (1 - alpha) + current.y * alpha;
        return interpolated;
    }

    void operator()(entity_t entity, sparse_array<Position>& positions, sparse_array<Velocity>& velocities) override {

        auto posOpt = positions[entity];
        auto velOpt = velocities[entity];

        if (posOpt.has_value() && velOpt.has_value()) {
            auto& pos = *positions[entity];
            auto& vel = *velocities[entity];
            pos.x += vel.x * deltaTime * 100.0f;
            pos.y += vel.y * deltaTime * 100.0f;
            Position prevPos = pos;

            pos.x += vel.x * deltaTime;
            pos.y += vel.y * deltaTime;

            // Debug print after applying velocity

            // Interpolate between previous and current position
            float alpha = deltaTime / (1.0f / 60.0f); // Assuming 60 logic updates per second
            pos = interpolate(prevPos, pos, alpha);

            // Debug print after interpolation
        }
    }

    void PostUpdate() override {
        // Handle heart beat events
        if (!heartBeatEvents.empty()) {
            Event event = heartBeatEvents.front();
            heartBeatEvents.pop();
            //this will containt the id of the client and the position we will use registry.clientToEntityMap
            std::vector<std::string> eventContent = tokenize(event.content, ":");
            //print every token
            for (auto &token : eventContent) {
            }
            entity_t entity = registry.clientToEntityMap[std::stoi(eventContent[0])];
            if (entity != 0) {
                //will directly apply the position from the event to the entity
                auto& posOpt = registry.get_components<Position>()[entity];
                if (posOpt.has_value()) {
                    auto& pos = *registry.get_components<Position>()[entity];
                    Position eventPos{std::stof(eventContent[1]), std::stof(eventContent[2])};

                    // Save current position as previous before updating
                    Position prevPos = pos;

                    // Set the position from the event
                    pos.x = eventPos.x;
                    pos.y = eventPos.y;

                    // Interpolate between previous and current position
                    float alpha = deltaTime / (1.0f / 60.0f); // Assuming 60 logic updates per second
                    pos = interpolate(prevPos, pos, alpha);
                }

            }

        }
    }
};


#endif /* !VELOCITYSYSTEM_HPP_ */