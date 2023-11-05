/*
** EPITECH PROJECT, 2023
** Rtype
** File description:
** GUIConfig.hpp
*/

/**
** GUIConfig
**
** This class represents the graphical user interface configuration.
**
** Public Methods:
**
** - GUIConfig()
**   - Default constructor
**
** - ~GUIConfig()
**   - Destructor
**
** - setResolution()
**   - Sets the GUI resolution
**
** - setTitle()
**   - Sets the GUI window title
**
** - setFPS()
**   - Sets the GUI frame rate
**
** - setTheme()
**   - Sets the GUI theme
**
** - getResolution()
**   - Gets the GUI resolution
**
** - getTitle()
**   - Gets the GUI window title
**
** - getFPS()
**   - Gets the GUI frame rate
**
** - getTheme()
**   - Gets the GUI theme
**
** - load()
**   - Loads the GUI config from a libconfig Setting
**
** - print()
**   - Prints the GUI config
**
** - getConfig()
**   - Gets the GUI config Setting from a file
**
** Private Attributes:
**
** - res
**   - Resolution
**
** - title
**   - Window title
**
** - fps
**   - Frame rate
**
** - theme
**   - Theme
*/

#ifndef GUICONFIG_HPP_
	#define GUICONFIG_HPP_

#include <string>
#include <libconfig.h++>
#include <iostream>
using namespace std;

struct themeData {
    int id;
    string name;
};

class GUIConfig {
	public:
		GUIConfig();
		~GUIConfig();
        void setResolution(pair<int, int> res);
        void setTitle(string title);
        void setFPS(int fps);
        void setTheme(themeData theme);
        pair<int, int> getResolution() const;
        string getTitle() const;
        int getFPS() const;
        themeData getTheme() const;
        void load(const libconfig::Setting& guiSetting);
        void print();

        libconfig::Setting& getConfig(const std::string& file) {
            libconfig::Config cfg;

            // Read the file. If there is an error, report it and exit.
            try {
                cfg.readFile(file.c_str());
            }
            catch(const libconfig::FileIOException &fioex) {
                std::cerr << "I/O error while reading file." << std::endl;
            }
            catch(const libconfig::ParseException &pex) {
                std::cerr << "Parse error at " << pex.getFile() << ":" << pex.getLine()
                          << " - " << pex.getError() << std::endl;
            }

            // Get the GUI configuration
            libconfig::Setting& gui = cfg.lookup("GUI");
            return gui;
        }

	protected:
    pair<int, int> res;
    string title;
    int fps;
    themeData theme;
	private:
};

#endif /*GUICONFIG_HPP_*/