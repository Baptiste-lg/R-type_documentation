/*
** EPITECH PROJECT, 2023
** Rtype
** File description:
** BeatmapConfig.hpp
*/

/**
** BeatmapConfig
**
** This class represents a beatmap configuration.
**
** Public Methods:
**
** - BeatmapConfig()
**   - Default constructor
**
** - ~BeatmapConfig()
**   - Destructor
**
** - load()
**   - Loads beatmap config from libconfig Setting
**   - Takes const libconfig::Setting&
**
** - print()
**   - Prints beatmap config
**
** - getFolderPath()
**   - Returns folder path string
**
** - getName()
**   - Returns name string
**
** - getArtist()
**   - Returns artist string
**
** - getDifficulty()
**   - Returns difficulty int
**
** Public Attributes:
**
** - id
**   - Beatmap ID int
**
** Protected Attributes:
**
** - Name
**   - Name string
**
** - Artist
**   - Artist string
**
** - FolderPath
**   - Folder path string
**
** - Difficulty
**   - Difficulty int
**
** - BPM
**   - Beats per minute int
**
** - offset
**   - Offset int
**
** Private Attributes:
**
** - None
*/

#ifndef BEATMAPCONFIG_HPP_
	#define BEATMAPCONFIG_HPP_

#include <string>
#include <map>
#include <iostream>
#include <libconfig.h++>
using namespace std;
class BeatmapConfig {
	public:
        BeatmapConfig();
        ~BeatmapConfig();
        void load(const libconfig::Setting& beatmapSetting);
        void print();
        int id;
        string getFolderPath() const { return FolderPath; }
        string getName() const { return Name; }
        string getArtist() const { return Artist; }
        int getDifficulty() const { return Difficulty; }
    protected:
        string Name;
        string Artist;
        string FolderPath;
        int Difficulty;
        int BPM;
        int offset;
	private:

};

#endif /*BEATMAPCONFIG_HPP_*/