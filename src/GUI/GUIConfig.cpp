/*
** EPITECH PROJECT, 2023
** Rtype
** File description:
** GUIConfig.cpp
*/

/**
* GUIConfig.cpp
*
* Handles loading and storing configuration data for the GUI.
*
* Methods:
*
* - GUIConfig()
*   Default constructor.
*
* - ~GUIConfig()
*   Destructor.
*
* - load()
*   Loads GUI configuration from libconfig Setting.
*
* - print()
*   Prints GUI configuration to console.
*
* - getResolution()
*   Returns resolution pair.
*
* - getTitle()
*   Returns window title string.
*
* - getFPS()
*   Returns target FPS.
*
* - getTheme()
*   Returns theme data struct.
*
* - setResolution()
*   Sets resolution pair.
*
* - setFPS()
*   Sets target FPS.
*
* - setTheme()
*   Sets theme data struct.
*
* Members:
*
* - title
*   Window title string.
*
* - fps
*   Target frames per second.
*
* - res
*   Resolution pair (width, height).
*
* - theme
*   Theme data struct.
*
*/

#include "GUI/GUIConfig.hpp"

GUIConfig::GUIConfig() {
}

GUIConfig::~GUIConfig() {
}

void GUIConfig::load(const libconfig::Setting& guiSetting) {
    guiSetting.lookupValue("title", title);
    guiSetting.lookupValue("fps", fps);
    const libconfig::Setting& res = guiSetting["res"];
    res.lookupValue("width", this->res.first);
    res.lookupValue("height", this->res.second);
    const libconfig::Setting& theme = guiSetting["theme"];
    theme.lookupValue("id", this->theme.id);
    theme.lookupValue("name", this->theme.name);
}

void GUIConfig::print() {

}

pair<int, int> GUIConfig::getResolution() const {
    return res;
}

string GUIConfig::getTitle() const {
    return title;
}

int GUIConfig::getFPS() const {
    return fps;
}

themeData GUIConfig::getTheme() const {
    return theme;
}

void GUIConfig::setResolution(pair<int, int> res) {
    this->res = res;
}

void GUIConfig::setFPS(int fps) {
    this->fps = fps;
}

void GUIConfig::setTheme(themeData theme) {
    this->theme = theme;
}

