/*
** EPITECH PROJECT, 2024
** Rtype
** File description:
** Global.cpp
*/

/**
** Global.cpp
**
** This file contains the global variables and utility functions used throughout the project.
** It defines a global configuration map `configMap` and a utility function `tokenize` for
** splitting a string into tokens based on a specified delimiter.
**
** Variables:
**
** - configMap
**   An unordered map to store key-value pairs representing configuration settings.
**
** Functions:
**
** - tokenize(std::string message, std::string delimiter)
**   This function takes a message string and a delimiter string as arguments, and returns
**   a vector of strings where each string is a token obtained by splitting the message
**   string at each occurrence of the delimiter string.
**
** Dependencies:
**
** - "Global.hpp" for declarations of global variables and utility functions.
** - <unordered_map> for std::unordered_map.
** - <vector> for std::vector.
** - <string> for std::string.
**
*/

#include "Global.hpp"

std::unordered_map<std::string, std::string> configMap;
std::vector<std::string> tokenize(std::string message, std::string delimiter) {
    std::vector<std::string> tokens;
    size_t pos = 0;
    std::string token;
    while ((pos = message.find(delimiter)) != std::string::npos) {
        token = message.substr(0, pos);
        tokens.push_back(token);
        message.erase(0, pos + delimiter.length());
    }
    tokens.push_back(message);
    return tokens;
}