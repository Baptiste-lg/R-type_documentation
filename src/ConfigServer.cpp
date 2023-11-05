/*
** EPITECH PROJECT, 2023
** Rtype
** File description:
** ConfigServer.cpp
*/

/**
** ConfigServer.cpp
**
** Implements functionality for loading and reading from a configuration file for the server.
**
** Functions:
**
** - loadConfig(const std::string& filePath)
**   Opens and reads the specified configuration file, parsing key-value pairs and
**   storing them in a global map. Skips lines that begin with '#' as they are
**   considered comments.
**
** - readFromConfig(const std::string& key)
**   Searches the global configuration map for the specified key and
**   returns its associated value. Returns an empty string if the key is not found.
**
** Global Variables:
**
** - configMap
**   A map for storing configuration key-value pairs.
**
** Dependencies:
**
** - <fstream> and <iostream> for file input/output and console output respectively.
** - "ConfigServer.hpp" and "Global.hpp" for related declarations.
**
** Notes:
**
** - The configuration file format is assumed to be simple key-value pairs separated
**   by '=' on each line, with optional comments beginning with '#'.
** - It may be more encapsulated and object-oriented to have a ConfigServer class
**   that holds the configMap and related functions, rather than using global functions
**   and variables.
**
*/

#include "ConfigServer.hpp"
#include "Global.hpp"

void loadConfig(const std::string& filePath) {
    std::ifstream file(filePath);
    std::string line;
    if (!file.is_open())
        return;
    while (std::getline(file, line)) {
        if (line[0] == '#') continue;  // Skip comments
        std::istringstream is_line(line);
        std::string key;
        if (std::getline(is_line, key, '=')) {
            std::string value;
            if (std::getline(is_line, value)) {
                configMap[key] = value;
            }
        }
    }
}

std::string readFromConfig(const std::string& key) {
    auto it = configMap.find(key);
    if (it != configMap.end()) {
        return it->second;
    }
    return "";  // Key not found
}