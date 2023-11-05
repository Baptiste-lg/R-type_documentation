/*
** EPITECH PROJECT, 2024
** Rtype
** File description:
** EffectFunctions.hpp
*/

/**
* EffectsFunctions.hpp
*
* Declares effect function prototypes and map.
*
* Functions:
*
* - SpeedUp(Registry&)
*   Speed up effect.
*
* - spawnShip(Registry&)
*   Spawn ship effect.
*
* - addScore(Registry&)
*   Add score effect.
*
* Data structures:
*
* - EffectFunctionMap
*   Map of string effect names to functions.
*
*/

#ifndef EFFECTFUNCTIONS_HPP_
#define EFFECTFUNCTIONS_HPP_

#include <functional>
#include <unordered_map>
#include "../Component.hpp"
#include "../Registry.hpp"

extern std::function<void(Registry&)> SpeedUp;
extern std::function<void(Registry&)> spawnShip;
extern std::function<void(Registry&)> addScore;

extern std::unordered_map<std::string, std::function<void(Registry&)>> EffectFunctionMap;

#endif  // EFFECTFUNCTIONS_HPP_
