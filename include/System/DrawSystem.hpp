/*
** EPITECH PROJECT, 2024
** Rtype
** File description:
** DrawSystem.hpp
*/

/**
* DrawSystem.hpp
*
* System to handle drawing of entities.
* Inherits from ISystem.
* Requires Drawable, Position, Depth and TextDrawable components.
*
* Methods:
*
* - DrawSystem()
*   Constructor, stores references to window, asset manager and registry.
*
* - sortEntities()
*   Sorts entities by depth.
*
* - operator()()
*   System update method. Draws entities.
*
* - invalidateSort()
*   Call when Depth changes to resort next frame.
*
* Members:
*
* - sortedEntities
*   Sorted draw order.
*
* - window
*   Reference to SFML window.
*
* - assetManager
*   Reference to asset manager.
*
* - registry
*   Reference to global ECS registry.
*
* - firstRun
*   Flag to track first run.
*
* - spritePool
*   Sprite pool for reuse.
*
* - deltaTime
*   Reference to game delta time.
*
*/

#ifndef DRAWSYSTEM_HPP_
#define DRAWSYSTEM_HPP_
#include "../include/System/ISystem.hpp"
#include "../include/System/System.hpp"
#include "../include/TextureManager.hpp"
#include "../include/Registry.hpp"
#include "../include/SpritePool.hpp"

class DrawSystem : public ISystem<Drawable, Position, Depth, TextDrawable> {
private:
    std::vector<entity_t> sortedEntities;  // Store sorted entities
    sf::RenderWindow& window;
    AssetManager& assetManager;
    Registry& registry;
    bool firstRun = true;
    SpritePool spritePool;
    float alpha = 0.0f;
    float &deltaTime;

public:
    DrawSystem(sf::RenderWindow& window, AssetManager& assetManager, Registry& registry, float& deltaTime)
            : window(window), assetManager(assetManager), registry(registry), deltaTime(deltaTime) {
        invalidateSort();

    }
    std::string const getSystemName() override {
        return "DrawSystem";
    }
    void sortEntities(sparse_array<Drawable>& drawables, sparse_array<Depth>& depths, sparse_array<TextDrawable>& textDrawables) {
        sortedEntities.clear();
        for (entity_t entity = 0; entity < drawables.size(); ++entity) {
            if ((drawables.is_active(entity) || textDrawables.is_active(entity)) && depths[entity]) {
                sortedEntities.push_back(entity);
            }
        }

        std::sort(sortedEntities.begin(), sortedEntities.end(), [&depths, &drawables](entity_t a, entity_t b) {
            if (depths[a]->z == depths[b]->z) {
                std::string aTexture = drawables[a] ? drawables[a]->textureName : "";
                std::string bTexture = drawables[b] ? drawables[b]->textureName : "";
                return aTexture < bTexture;
            }
            return depths[a]->z < depths[b]->z;
        });

    }

    void operator()(entity_t entity, sparse_array<Drawable>& drawables)  {
    }

    void operator()(entity_t entity, sparse_array<Drawable>& drawables, sparse_array<Position>& positions, sparse_array<Depth>& depths, sparse_array<TextDrawable>& textDrawables) override {
        if (firstRun) {
            sortEntities(drawables, depths, textDrawables);
            for (entity_t sortedEntity : sortedEntities) {
                auto drawableOpt = drawables[sortedEntity];
                auto posOpt = positions[sortedEntity];
                auto depthOpt = depths[sortedEntity];
                auto textDrawableOpt = textDrawables[sortedEntity];

                if (drawableOpt && posOpt && depthOpt) {
                    // Extract the actual components
                    auto& drawable = *drawableOpt;
                    auto& pos = *posOpt;

                    // Get a sprite from the pool
                    sf::Sprite* sprite = spritePool.getSprite();
                    sprite->setTexture(assetManager.GetTexture(drawable.textureName));
                    sprite->setPosition(pos.x, pos.y);
                    sprite->setScale(drawable.scale.x, drawable.scale.y);
                    sprite->setTextureRect(sf::IntRect(drawable.textureRect.left, drawable.textureRect.top, drawable.textureRect.width, drawable.textureRect.height));

#ifdef RENDER_HITBOX
                    // Hitbox rendering code
                        sf::RectangleShape shape;
                        shape.setFillColor(sf::Color::Transparent);
                        shape.setOutlineColor(sf::Color::Red);
                        shape.setOutlineThickness(1);
                        shape.setPosition(sf::Vector2f(pos.x, pos.y));
                        shape.setSize(sf::Vector2f(drawable.textureRect.width, drawable.textureRect.height));
                        window.draw(shape);
#endif
                    window.draw(*sprite);
                }
                if (textDrawableOpt && posOpt) {
                    auto& textDrawable = *textDrawableOpt;
                    auto& pos = *posOpt;

                    sf::Text text;
                    text.setFont(assetManager.GetFont(textDrawable.fontName));  // Set the font
                    text.setString(textDrawable.text);  // Set the string
                    text.setCharacterSize(textDrawable.fontSize);  // Set the character size
                    text.setFillColor(textDrawable.color);  // Set the color

                    // Get the bounding box of the text
                    sf::FloatRect textBounds = text.getLocalBounds();

                    // Get the dimensions of the sprite
                    float spriteWidth = 100.0f;
                    float spriteHeight  = 100.0f;
                    if (drawableOpt)
                    {
                        auto& drawable = *drawableOpt;
                        spriteWidth = drawable.textureRect.width * drawable.scale.x;
                        spriteHeight = drawable.textureRect.height * drawable.scale.y;
                    }
                    // Scale the text to fit within the sprite, if necessary
                    if (textBounds.width > spriteWidth || textBounds.height > spriteHeight) {
                        float xScale = spriteWidth / textBounds.width;
                        float yScale = spriteHeight / textBounds.height;
                        float minScale = std::min(xScale, yScale);
                        text.setCharacterSize(static_cast<unsigned int>(textDrawable.fontSize * minScale));
                    }

                    // Re-calculate the bounding box if you've changed the character size
                    textBounds = text.getLocalBounds();

                    // Position the text in the middle of the sprite
                    text.setPosition(pos.x + (spriteWidth - textBounds.width) / 2, pos.y + (spriteHeight - textBounds.height) / 2);

                    window.draw(text);
                }
            }

            firstRun = false;
        }
    }


    // Call this whenever you modify the Depth component of any entity
    void invalidateSort() {
        firstRun = true;
    }
    void PostUpdate() override {
        invalidateSort();
        spritePool.clear();
    }
};


#endif /* !DRAWSYSTEM_HPP_ */