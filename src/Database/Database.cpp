/*
** EPITECH PROJECT, 2023
** Rtype
** File description:
** Database.cpp
*/

/**
** Database.cpp
**
** This file implements the Database class which loads and stores
** beatmap configuration data.
**
** Methods:
**
** - Database()
**   Default constructor.
**
** - ~Database()
**   Destructor.
**
** - load()
**   Loads beatmap data from a config file.
**   Parses the file and handles errors.
**   Stores BeatmapConfig objects in a map.
**
** - updateDatabase()
**   Clears and reloads the database.
**   Returns the ID of the last loaded beatmap.
**
** - print()
**   Prints all stored BeatmapConfigs.
**
** - isBeatmapExist()
**   Checks if a beatmap ID or path exists in the database.
**
** - getBeatmapByName()
**   Gets the ID of a beatmap by name.
**
** Members:
**
** - beatmaps
**   Map storing BeatmapConfigs by ID.
**
*/

#include "Database/Database.hpp"

Database::Database()
{
}

Database::~Database()
{
}

void Database::load(const std::string& file) {
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

    // Get the songs configuration
    const libconfig::Setting& songs = cfg.lookup("songs");
    for (int i = 0; i < songs.getLength(); ++i) {
        BeatmapConfig beatmapConfig;
        beatmapConfig.load(songs[i]);
        beatmaps[beatmapConfig.id] = beatmapConfig;
    }
}

int Database::updateDatabase(const std::string& folderPath) {
//unload everything, to fully reload it and return the last id
    beatmaps.clear();
    load(folderPath);
    int id =-1;
    if (beatmaps.size() > 0) {
        id = beatmaps.size() - 1;
    }
    return id;
}

void Database::print()
{
    for (auto it = beatmaps.begin(); it != beatmaps.end(); ++it) {
        it->second.print();
    }
}

bool Database::isBeatmapExist(int id) {
    auto it = beatmaps.find(id);
    if (it == beatmaps.end()) {
        // Beatmap not found in the database
        return false;
    }

    std::filesystem::path path("asset/Beatmaps/" + it->second.getFolderPath());
    if (!std::filesystem::exists(path)) {
        // Beatmap folder does not exist
        beatmaps.erase(it);
        return false;
    }

    // Beatmap exists in the database and its folder exists
    return true;
}


bool Database::isBeatmapExist(const std::string& folderPath) {
    for (auto it = beatmaps.begin(); it != beatmaps.end(); ++it) {
        if (it->second.getFolderPath() == folderPath) {
            return true;
        }
    }
    return false;
}

int Database::getBeatmapByName(const std::string& name) {
    for (auto it = beatmaps.begin(); it != beatmaps.end(); ++it) {
		if (it->second.getName() == name) {
			return it->first;
		}
	}
    return -1;
}