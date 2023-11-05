/*
** EPITECH PROJECT, 2024
** Rtype
** File description:
** global
*/

/**
** Global
**
** This file contains global definitions and variables.
**
** Defines:
**
** - FRAMERATE
**   Frames per second rate.
**
** Variables:
**
** - configMap
**   Global map containing loaded configuration values.
**
** Functions:
**
** - tokenize()
**   Tokenizes a string into a vector of tokens.
**   Takes message string and delimiter string.
**   Returns vector of token strings.
**
*/

#ifndef GLOBAL_HPP_
#define GLOBAL_HPP_

#include <SFML/Graphics.hpp>
#include <stdio.h>
#include <unordered_map>
#include <iostream>

#define FRAMERATE 60
extern std::unordered_map<std::string, std::string> configMap;
std::vector<std::string> tokenize(std::string message, std::string delimiter);

#endif /* !GLOBAL_HPP_ */