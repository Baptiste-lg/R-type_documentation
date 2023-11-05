/*
** EPITECH PROJECT, 2024
** Rtype
** File description:
** EffectsFunctions.cpp
*/

/**
** EffectsFunctions.cpp
**
** This file contains definitions of various effect functions which are meant to be
** triggered during the game. It utilizes C++ functional library to create a map
** between string identifiers and corresponding functional objects. These functions
** primarily interact with the game's registry to access or modify game configuration
** and state.
**
** Functions:
**
** - SpeedUp
**   This function is designed to speed up the player's movement by modifying
**   the playerSpeed attribute of the GameConfig component. If the GameConfig
**   component is not set, it outputs an error message to the standard error stream.
**
** - addScore
**   This function adds a fixed amount of score to the player's current score
**   by modifying the playerScore attribute of the GameConfig component.
**   Similar to SpeedUp, it checks whether the GameConfig component is set
**   and outputs an error message to the standard error stream if it's not set.
**
** - spawnShip
**   This function is a placeholder for logic related to spawning a new ship in the game.
**
** Global Variables:
**
** - EffectFunctionMap
**   This is an unordered map which maps string identifiers to the corresponding
**   effect functions. This map allows for the effect functions to be looked up and
**   invoked dynamically based on string identifiers.
**
** Dependencies:
**
** - "../include/System/EffectsFunctions.hpp" for related declarations.
** - <functional> for std::function.
** - <unordered_map> for std::unordered_map.
**
*/

#include "../include/System/EffectsFunctions.hpp"

std::function<void(Registry&)> SpeedUp = [](Registry& registry) {
    // Access the GameConfig component array
    auto &gameConfigOpt = registry.get_components<GameConfig>()[registry.stateEntity];
    if (!gameConfigOpt.has_value()) {
        std::cerr << "GameConfig not set. Exiting SpeedUp." << std::endl;
            // Access the optional GameConfig component


    } else {
        auto &config = *gameConfigOpt;
        // Check if the optional has a value
        // Access the GameConfig component and modify playerSpeed, e.g., double it
        config.playerSpeed *= 4;
    }

};

std::function<void(Registry&)> addScore = [](Registry& registry) {
    // Access the GameConfig component array
    auto &gameConfigOpt = registry.get_components<GameConfig>()[registry.stateEntity];
    if (gameConfigOpt.has_value()) {
        auto &config = *gameConfigOpt;
        config.playerScore += 15;
    } else {
        std::cerr << "GameConfig not set. Exiting addScore." << std::endl;
    }
};

std::function<void(Registry&)> spawnShip = [](Registry& registry) {
    // Your logic here...
};

std::unordered_map<std::string, std::function<void(Registry&)>> EffectFunctionMap = {
    {"SpeedUp", SpeedUp},
    {"SpawnShip", spawnShip},
    {"addScore", addScore}

        // ... add other death effects here ...
};
