/*
** EPITECH PROJECT, 2023
** Rtype
** File description:
** Config.hpp
*/

/**
** Config
**
** This class represents the configuration for the game.
**
** Public Methods:
**
** - Config()
**   - Default constructor
**
** - ~Config()
**   - Destructor
**
** - load()
**   - Loads the configuration from a file
**
** - save()
**   - Saves the configuration to a file
**
** - print()
**   - Prints the configuration
**
** - getGUIConfig()
**   - Returns the GUI configuration
**
** Private Attributes:
**
** - path
**   - The path to the configuration file
**
** - guiConfig
**   - The GUI configuration
**
** Private Methods:
**
** - None
*/

#ifndef CONFIG_HPP_
	#define CONFIG_HPP_
#include <string>
#include <map>
#include <libconfig.h++>
#include <iostream>
#include "GUI/GUIConfig.hpp"
#include "Beatmap/BeatmapConfig.hpp"
using namespace std;

class Config {
public:
    Config();
    ~Config();
    void load(const std::string& file);
    void save(const std::string& file);
    void print();
    GUIConfig getGUIConfig() const;
private:
    // Static members to store the configuration values
    string path;
    GUIConfig guiConfig;
};

#endif /*CONFIG_HPP_*/

