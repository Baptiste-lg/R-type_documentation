/*
** EPITECH PROJECT, 2024
** Rtype
** File description:
** SYSTEM
*/

/**
 * ClickableSystem processes entities with isClickable and Position components.
 *
 * It checks for mouse clicks within the entity's position/size.
 * Applies a short cooldown before triggering clickable.onClick().
 *
 * This allows easily making UI elements and sprites clickable.
 *
 * Usage:
 * - Give entities isClickable and Position components.
 * - Implement onClick() function in isClickable.
 * - System will check for clicks and call onClick().
 *
 * Parameters:
 * - window: Reference to the SFML window.
 * - registry: Reference to the ECS registry.
 * - deltaTime: Time elapsed since last tick.
 *
 * Components used:
 * - isClickable: Contains onClick() callback.
 * - Position: Used to check click is within bounds.
 * - Drawable (optional): Used for width/height.
*/

#ifndef CLICKABLESYSTEM_HPP_
#define CLICKABLESYSTEM_HPP_
#include "../include/System/ISystem.hpp"
#include "../include/System/System.hpp"
#include "../include/TextureManager.hpp"
#include "../include/Registry.hpp"
#include "../include/Event.hpp"



class ClickSystem : public ISystem<isClickable, Position> {
private:
    sf::RenderWindow& window;
    Registry& registry;
    float& deltaTime;
public:
    std::string const getSystemName() override {
        return "ClickSystem";
    }

    ClickSystem(sf::RenderWindow& window, Registry& registry, float& deltaTime) : window(window), registry(registry), deltaTime(deltaTime) {}

    void operator()(entity_t entity, sparse_array<isClickable>& clickables, sparse_array<Position>& positions) override {
        auto clickableOpt = clickables[entity];
        auto posOpt = positions[entity];
        auto drawableOpt = registry.get_components<Drawable>()[entity];  // Assuming yourRegistry is available

        if (clickableOpt && posOpt) {
            auto& clickable = *clickables[entity];
            auto& pos = *positions[entity];
            float width = drawableOpt ? drawableOpt->textureRect.width : 50.0f;  // Replace 50.0f with some default
            float height = drawableOpt ? drawableOpt->textureRect.height : 50.0f;  // Replace 50.0f with some default

            sf::Vector2i mousePos = sf::Mouse::getPosition(window);  // Get mouse position
            bool isMousePressed = sf::Mouse::isButtonPressed(sf::Mouse::Left);  // Check for left click
#ifdef RENDER_HITBOX
            std::cout << "mousePos.x: " << mousePos.x << std::endl;
            std::cout << "mousePos.y: " << mousePos.y << std::endl;
#endif

            clickable.elapsedTimeSinceLastClick += deltaTime;

            if (isMousePressed && mousePos.x >= pos.x && mousePos.x <= pos.x + width && mousePos.y >= pos.y && mousePos.y <= pos.y + height) {
                // Don't immediately set isClicked to true; check elapsed time
                if (clickable.elapsedTimeSinceLastClick >= 0.2f) {  // assuming 0.02s cooldown
                    clickable.isClicked = true;
                    // Reset the elapsed time only here
                    clickable.elapsedTimeSinceLastClick = 0.0f;
                }
            }

            if (clickable.isClicked) {
                clickable.onClick();
                clickable.isClicked = false;  // Reset after handling
            }
        }
    }
};




#endif /* !CLICKABLESYSTEM_HPP_ */
