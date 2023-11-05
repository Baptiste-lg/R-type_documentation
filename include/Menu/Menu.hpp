/*
** EPITECH PROJECT, 2023
** Rtype
** File description:
** Menu.hpp
*/

/**
** Menu
**
** This class represents the main menu.
**
** Public Methods:
**
** - Menu()
**   - Default constructor
**
** - ~Menu()
**   - Destructor
**
** - load()
**   - Loads the menu resources
**
** - loadTextures()
**   - Loads the menu textures
**
** - update()
**   - Updates the menu and handles events
**
** - draw()
**   - Draws the menu
**
** - loadBeatmaps()
**   - Loads the beatmap panels
**
** - slideLeft()
**   - Slides the beatmap panels left
**
** - slideRight()
**   - Slides the beatmap panels right
**
** - onAnalyzeButtonClicked()
**   - Handles analyze button click
**
** - getDifficulty()
**   - Gets selected difficulty
**
** - getActualSongName()
**   - Gets selected song name
**
** - setDifficulty()
**   - Sets selected difficulty
**
** - setState()
**   - Sets the menu state
**
** Protected Attributes:
**
** - buttons
**   - Menu buttons
**
** - buttonConfigs
**   - Button configurations
**
** - textures
**   - Button textures
**
** - sprites
**   - Menu sprites
**
** - beatmapPanel
**   - Beatmap panels
**
** - fonts
**   - Loaded fonts
**
** - state
**   - Current menu state
**
** Private Attributes:
**
** - None
*/

#ifndef MENU_HPP_
	#define MENU_HPP_

#include <SFML/Graphics.hpp>
#include <string>
#include <vector>
#include <tuple>
#include <vector>
#include <tuple>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
using namespace std;
#include "GraphicElement/Button.hpp"
#include "Database/Database.hpp"
#include "GraphicElement/BeatmapPanel.hpp"
//#include "GraphicElement/TriangleButton.hpp"
//TODO: BANNER
enum MenuState {
    SONGSELECT,
    PLAYWAITING,
    BACKWAITING
};

class Menu {
public:
    Menu();
    ~Menu();
    void load(const std::pair<int, int>& res, Database& database);
    void loadTextures(Database& database);
    int update(const sf::Event& event, const sf::RenderWindow& window);
    void draw(sf::RenderWindow& window);
    void loadBeatmaps(Database& database);
    void slideLeft();
    void slideRight();
    void onAnalyzeButtonClicked(const std::string& beatmapPath);
    int getDifficulty();
    std::string getActualSongName();
    void setDifficulty(int difficulty);
    void setState(int nState){
        this->state = nState;
    }
protected:
    int selectedPanelIndex = 0; // Add this line
    map<string, Button> buttons;
    map<string, ButtonConfig> buttonConfigs;
    map<string, sf::Text> texts;
    map<string, sf::Texture> textures;
    map<string, sf::Sprite> sprites;
    //add the beatmap panel
    std::vector<std::unique_ptr<BeatmapPanel>> beatmapPanel;
    map<string, sf::Font> fonts;
    int state = SONGSELECT;
    pair<int, int> _res = {0, 0};
    int theme = 0;
    //add the fft
private:
};

#endif /*MENU_HPP_*/