/*
** EPITECH PROJECT, 2024
** Rtype
** File description:
** SYSTEM
*/

/**
 * AnimationSystem processes Animation and Drawable components each frame.
 *
 * It tracks elapsed time and updates the current animation frame when the frame
 * duration has elapsed. This updates the texture rect on the Drawable component
 * if it exists.
 *
 * Parameters:
 * - e: The entity being processed.
 * - animations: Sparse array of Animation components.
 * - drawables: Sparse array of Drawable components.
 *
 * Usage:
 * - Get the Animation component for the entity.
 * - Increment the elapsed time by deltaTime.
 * - Check if elapsed time exceeds current frame duration.
 *   - Reset elapsed time to 0.
 *   - Increment current frame, handling end of animation or looping logic.
 * - If Drawable component exists, update its texture rect to the new frame.
*/

#ifndef ANIMATIONSYSTEM_HPP_
#define ANIMATIONSYSTEM_HPP_
#include "../include/System/ISystem.hpp"
#include "../include/System/System.hpp"
#include "../include/TextureManager.hpp"
#include "../include/Registry.hpp"
#include "../include/Event.hpp"
#include <unistd.h>
#include <string.h>
#include <unordered_map>
#include <fcntl.h>
class AnimationSystem : public ISystem<Animation, Drawable> {
private:
    float& deltaTime;

public:
    std::string const getSystemName() override {
        return "AnimationSystem";
    }
    AnimationSystem(float& deltaTime) : deltaTime(deltaTime) {}

    void operator()(entity_t e, sparse_array<Animation>& animations, sparse_array<Drawable>& drawables) override {

        auto animOpt = animations[e];
        auto drawableOpt = drawables[e];
        if (animOpt.has_value()) {
            auto& anim = *animations[e];
            anim.elapsedTime += deltaTime;

            if (anim.elapsedTime >= anim.frameDuration) {
                anim.elapsedTime = 0.0f;

                if (anim.loop) {
                    // Normal looping behavior
                    anim.currentFrame = (anim.currentFrame + 1) % anim.numFrames;
                } else {
                    // If not looping, stop at the last frame
                    if (anim.currentFrame < anim.numFrames - 1) {
                        anim.currentFrame++;
                    }
                }

                // Update the texture rect if drawable component exists
                if (drawableOpt.has_value()) {
                    auto& drawable = *drawables[e];
                    drawable.textureRect.left = drawable.textureRect.width * anim.currentFrame;
                }
            }
        }
    }
};


#endif /* !ANIMATIONSYSTEM_HPP_ */