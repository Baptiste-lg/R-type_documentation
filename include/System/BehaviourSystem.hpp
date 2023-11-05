/*
** EPITECH PROJECT, 2024
** Rtype
** File description:
** SYSTEM
*/

/**
 * BehaviorSystem processes entities with Behavior and Position components.
 *
 * It looks up the behavior function based on the Behavior.behaviorID
 * and executes it, passing in the Position component and deltaTime.
 *
 * This allows easily swapping out movement behaviors for entities.
 *
 * Usage:
 * - Give entities a Behavior and Position component.
 * - Implement movement functions like moveLeft(), zigzagMovement() etc.
 * - Add function pointer to behaviorMap.
 * - System will call correct function each tick to update Position.
 *
 * Parameters:
 * - deltaTime: Time elapsed since last tick.
 *
 * Components used:
 * - Behavior: Contains behaviorID to identify movement function.
 * - Position: Passed to behavior function to update.
*/

#ifndef BEHAVIOURSYSTEM_HPP_
#define BEHAVIOURSYSTEM_HPP_
#include "../include/System/ISystem.hpp"
#include "../include/System/System.hpp"
#include "../include/TextureManager.hpp"
#include "../include/Behaviour.hpp"
#include "../include/Registry.hpp"
#include "../include/Event.hpp"
#include <unistd.h>
#include <string.h>
#include <unordered_map>
#include <fcntl.h>
std::unordered_map<std::string, BehaviorFunc> behaviorMap = {
        {"MoveLeft", moveLeft},
        {"Zigzag", zigzagMovement},
        {"Circular", circularMovement},
        {"Diagonal", diagonalMovement},
        {"SineWave", sineWaveMovement},
        {"BackAndForth", backAndForthMovement},
        {"LShape", lShapeMovement},
        {"Random", randomMovement},
        {"SliderLoopFix", sliderLoopFixMovement},
        {"Fixed", fixedMovement},
        {"LoopDiagonalRight", loopDiagonalMovementRight}
        // Add more behaviors here
};
class BehaviorSystem : public ISystem<Behavior, Position> {
private:
    float& deltaTime;

public:
    BehaviorSystem(float& deltaTime) : deltaTime(deltaTime) {}
    std::string const getSystemName() override {
        return "BehaviorSystem";
    }
    void operator()(entity_t e, sparse_array<Behavior>& behaviors, sparse_array<Position>& positions) override {

        auto behaviorOpt = behaviors[e];
        auto posOpt = positions[e];

        if (behaviorOpt && posOpt) {
            auto& behavior = *behaviors[e];
            auto& pos = *positions[e];

            // Look up the behavior function from the map
            auto it = behaviorMap.find(behavior.behaviorID);
            if (it != behaviorMap.end()) {
                // Execute the behavior function
                it->second(pos, deltaTime);
            }
        }
    }
};


#endif /* !BEHAVIOURSYSTEM_HPP_ */