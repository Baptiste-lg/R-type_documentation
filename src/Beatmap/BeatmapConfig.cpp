/*
** EPITECH PROJECT, 2023
** Rtype
** File description:
** BeatmapConfig.cpp
*/

/**
** BeatmapConfig.cpp
**
** This file implements the BeatmapConfig class which holds configuration
** data for a beatmap.
**
** Methods:
**
** - BeatmapConfig()
**   Default constructor.
**
** - ~BeatmapConfig()
**   Destructor.
**
** - load()
**   Loads configuration data from a libconfig Setting.
**
** - print()
**   Prints the configuration data to stdout.
**
** Members:
**
** - Name
**   Name of the beatmap.
**
** - Artist
**   Artist of the song.
**
** - FolderPath
**   Path to audio files.
**
** - Difficulty
**   Difficulty level.
**
** - BPM
**   Beats per minute of the song.
**
** - id
**   Beatmap ID.
**
** - offset
**   Audio offset.
**
*/

#include "Beatmap/BeatmapConfig.hpp"

BeatmapConfig::BeatmapConfig()
{
}

BeatmapConfig::~BeatmapConfig()
{
}

void BeatmapConfig::load(const libconfig::Setting& beatmapSetting) {
    beatmapSetting.lookupValue("Name", Name);
    beatmapSetting.lookupValue("Artist", Artist);
    beatmapSetting.lookupValue("FolderPath", FolderPath);
    beatmapSetting.lookupValue("Difficulty", Difficulty);
    beatmapSetting.lookupValue("BPM", BPM);
    beatmapSetting.lookupValue("id", id);
    beatmapSetting.lookupValue("offset", offset);
}

void BeatmapConfig::print()
{
}