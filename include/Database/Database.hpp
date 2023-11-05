/*
** EPITECH PROJECT, 2023
** Rtype
** File description:
** Database.hpp
*/

/**
** Database
**
** This class represents the game's database of beatmaps.
**
** Public Methods:
**
** - Database()
**   - Constructor
**
** - ~Database()
**   - Destructor
**
** - load()
**   - Loads the database from a file
**
** - print()
**   - Prints the database
**
** - isBeatmapExist()
**   - Checks if a beatmap exists by ID or path
**
** - getNbBeatmaps()
**   - Gets the number of beatmaps
**
** - getBeatmap()
**   - Gets a beatmap config by ID
**
** - getBeatmapByName()
**   - Gets a beatmap ID by name
**
** - updateDatabase()
**   - Updates the database from a folder path
**
** Protected Attributes:
**
** - beatmaps
**   - Map of beatmap configs by ID
**
** Private Methods:
**
** - None
*/

#ifndef DATABASE_HPP_
	#define DATABASE_HPP_
#include "Beatmap/BeatmapConfig.hpp"
#include <iostream>
#include <string>
#include <filesystem>
#include <map>
using namespace std;

class Database {
	public:
        Database();
        ~Database();
        void load(const std::string& file);
        void print();
        bool isBeatmapExist(int id);
        int getNbBeatmaps() const { return beatmaps.size(); }
        BeatmapConfig getBeatmap(int id) { return beatmaps[id]; }
        bool isBeatmapExist(const std::string& folderPath);
        int getBeatmapByName(const std::string& name);
        int updateDatabase(const std::string& folderPath);
	protected:
        map<int, BeatmapConfig> beatmaps;
	private:
};

#endif /*DATABASE_HPP_*/