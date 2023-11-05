/*
** EPITECH PROJECT, 2023
** Rtype
** File description:
** Config.cpp
*/

/**
** Config.cpp
**
** This file implements the Config class which loads configuration
** data from a file.
**
** Methods:
**
** - Config()
**   Default constructor.
**
** - ~Config()
**   Destructor.
**
** - load()
**   Loads configuration from a file.
**   Parses the file with libconfig and handles errors.
**   Loads GUI and player configurations.
**
** - print()
**   Prints the loaded configuration.
**
** - getGUIConfig()
**   Getter for the GUIConfig member.
**
** Members:
**
** - guiConfig
**   Stores the loaded GUI configuration.
**
*/

#include "Config/Config.hpp"

Config::Config() {
}

Config::~Config() {
}


void Config::load(const std::string& file) {
    libconfig::Config cfg;

    // Read the file. If there is an error, report it and exit.
    try {
        cfg.readFile(file.c_str());
    }
    catch(const libconfig::FileIOException &fioex) {
        std::cerr << "I/O error while reading file." << std::endl;
        return;
    }
    catch(const libconfig::ParseException &pex) {
        std::cerr << "Parse error at " << pex.getFile() << ":" << pex.getLine()
                  << " - " << pex.getError() << std::endl;
        return;
    }

    // Get the GUI configuration
    const libconfig::Setting& gui = cfg.lookup("GUI");
    guiConfig.load(gui);

    // Get the player configuration
}

void Config::print() {
    guiConfig.print();
}

GUIConfig Config::getGUIConfig() const {
    return guiConfig;
}