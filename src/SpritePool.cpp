/*
** EPITECH PROJECT, 2024
** Rtype
** File description:
** SpritePool.cpp
*/

/**
** SpritePool.cpp
**
** This file defines the implementation of the SpritePool class, which is designed to manage a pool of sprite objects.
** It provides mechanisms to obtain a sprite from the pool and to return a sprite back to the pool after use.
** This pool management helps in reusing sprite objects, thereby improving the performance and reducing the memory allocations.
**
** Functions:
**
** - sf::Sprite* SpritePool::getSprite():
**   This function checks if there are any available sprites in the pool.
**   If the pool is empty, it creates a new sprite, adds it to the available pool, and then moves it to the in-use pool before returning it.
**   If there are available sprites, it simply moves the first available sprite from the available pool to the in-use pool and returns it.
**
** - void SpritePool::returnSprite(sf::Sprite* sprite):
**   This function takes a sprite pointer as an argument, removes this sprite from the in-use pool,
**   and adds it back to the available pool for future use.
**
** Dependencies:
**
** - "SpritePool.hpp" for the declaration of the SpritePool class and its methods.
** - <SFML/Graphics.hpp> for the sf::Sprite class.
**
*/

#include "SpritePool.hpp"

sf::Sprite* SpritePool::getSprite() {
    if (available.empty()) {
        available.push_back(new sf::Sprite());
    }
    sf::Sprite* sprite = available.front();
    inUse.push_back(sprite);
    available.pop_front();
    return sprite;
}

void SpritePool::returnSprite(sf::Sprite* sprite) {
    inUse.remove(sprite);
    available.push_back(sprite);
}
