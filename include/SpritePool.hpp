/*
** EPITECH PROJECT, 2024
** Rtype
** File description:
** sprite pool
*/

/**
 * SpritePool.hpp defines a pool for reusing SFML sprites.
 *
 * The sprite pool prevents constantly allocating and freeing sprites.
 * Sprites are reused from an available pool instead of created/destroyed.
 *
 * This improves performance by reducing memory allocations.
 *
 * Usage:
 * - GetSprite(): Gets a sprite from the available pool.
 * - ReturnSprite(): Returns a sprite to the pool when done.
 *
 * Methods:
 * - getSprite(): Moves a sprite from available to inUse pool.
 * - returnSprite(): Moves it back to available.
 *
 * Pools:
 * - available: List of unused sprites.
 * - inUse: List of sprites currently in use.
*/

#ifndef SPRITEPOOL_HPP_
#define SPRITEPOOL_HPP_
#include <list>
#include <SFML/Graphics.hpp>

class SpritePool {
public:
    sf::Sprite* getSprite();
    void returnSprite(sf::Sprite* sprite);

    void clear(){
        for (auto& sprite : available) {
            delete sprite;
        }
        for (auto& sprite : inUse) {
            delete sprite;
        }
        available.clear();
        inUse.clear();

    }

private:
    std::list<sf::Sprite*> available;
    std::list<sf::Sprite*> inUse;
};
#endif /* !SPRITEPOOL_HPP_ */