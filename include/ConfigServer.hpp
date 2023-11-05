/*
** EPITECH PROJECT, 2024
** Rtype
** File description:
** ConfigServer
*/

/**
** ConfigServer
**
** This file provides functions to load and read server configuration.
**
** Functions:
**
** - readFromConfig()
**   Reads a config value for the given key.
**   Takes key string.
**   Returns value string.
**
** - loadConfig()
**   Loads the server config from a file.
**   Takes config file path string.
**
*/

#ifndef CONFIGSERVER_HPP_
#define CONFIGSERVER_HPP_
#include <fstream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <iostream>


std::string readFromConfig(const std::string& key);
void loadConfig(const std::string& filePath);
#endif /* !CONFIGSERVER_HPP_ */
