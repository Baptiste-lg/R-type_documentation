/*
** EPITECH PROJECT, 2024
** Rtype
** File description:
** Behaviour.hpp
*/

/**
** Behaviour.hpp
**
** This file defines behavior functions for game entities.
**
** Behavior Functions:
**
** - moveLeft(Position&, float)
**   - Moves entity left
**
** - zigzagMovement(Position&, float)
**   - Moves entity in a zigzag pattern
**
** - sliderLoopFixMovement(Position&, float)
**   - Moves entity in a fixed loop pattern
**
** - circularMovement(Position&, float)
**   - Moves entity in a circular pattern
**
** - diagonalMovement(Position&, float)
**   - Moves entity diagonally
**
** - sineWaveMovement(Position&, float)
**   - Moves entity in a sine wave pattern
**
** - backAndForthMovement(Position&, float)
**   - Moves entity back and forth
**
** - lShapeMovement(Position&, float)
**   - Moves entity in an L-shape pattern
**
** - randomMovement(Position&, float)
**   - Moves entity randomly
**
** - fixedMovement(Position&, float)
**   - Moves entity in a fixed pattern
**
** - loopDiagonalMovementRight(Position&, float)
**   - Moves entity diagonally in a loop
*/

#ifndef BEHAVIOUR_HPP_
#define BEHAVIOUR_HPP_
#include "Component.hpp"
#include <functional>
#include <cmath>

//need to move to behavior.cpp
using BehaviorFunc = std::function<void(Position&, float)>;

void moveLeft(Position& pos, float deltaTime);
void zigzagMovement(Position& pos, float deltaTime);
void sliderLoopFixMovement(Position& pos, float deltaTime);
void circularMovement(Position& pos, float deltaTime);
void diagonalMovement(Position& pos, float deltaTime);
void sineWaveMovement(Position& pos, float deltaTime);
void backAndForthMovement(Position& pos, float deltaTime);
void lShapeMovement(Position& pos, float deltaTime);
void randomMovement(Position& pos, float deltaTime);
void fixedMovement(Position& pos, float deltaTime);
void loopDiagonalMovementRight(Position& pos, float deltaTime);
#endif /* !BEHAVIOUR_HPP_ */