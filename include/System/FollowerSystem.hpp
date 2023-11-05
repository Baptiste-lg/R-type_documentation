/*
** EPITECH PROJECT, 2024
** Rtype
** File description:
** SYSTEM
*/

/**
 * FollowerSystem processes entities with Follower and Position components.
 *
 * It updates the Position of followers to match their targets with an offset.
 *
 * This allows easily making entities follow other entities.
 *
 * Usage:
 * - Give follower entities Follower and Position components.
 * - Set Follower.target to the entity to follow.
 * - Set Follower.xOffset and Follower.yOffset for the offset.
 * - System will update Position each frame based on target's Position.
 *
 * Parameters:
 * - None
 *
 * Components used:
 * - Follower: Contains target and offset.
 * - Position: Updated to match target's Position + offset.
*/

#ifndef FOLLOWERSYSTEM_HPP_
#define FOLLOWERSYSTEM_HPP_
#include "../include/System/ISystem.hpp"
#include "../include/System/System.hpp"
#include "../include/TextureManager.hpp"
#include "../include/Registry.hpp"

class FollowerSystem : public ISystem<Follower, Position> {
public:
    FollowerSystem() = default;
    std::string const getSystemName() override {
        return "FollowerSystem";
    }
    void operator()(entity_t entity, sparse_array<Follower>& followers, sparse_array<Position>& positions) override {
        auto followerOpt = followers[entity];
        auto posOpt = positions[entity];
        if (followerOpt && posOpt) {
            auto posTargetOpt = positions[followerOpt->target];
            auto& follower = *followers[entity];
            auto& pos = *positions[entity];
            if (posTargetOpt) {
                auto& posTarget = *positions[follower.target];
                pos.x = posTarget.x + follower.xOffset;
                pos.y = posTarget.y + follower.yOffset;
                pos.prevX = pos.x;
                pos.prevY = pos.y;
            }
        }
    }
};


#endif /* !FOLLOWERSYSTEM_HPP_ */