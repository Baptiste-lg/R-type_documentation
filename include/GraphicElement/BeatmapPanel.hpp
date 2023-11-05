/*
** EPITECH PROJECT, 2023
** Rtype
** File description:
** BeatmapPanel.hpp
*/

/**
** BeatmapPanel
**
** This class represents a panel to display a beatmap's information.
**
** Public Methods:
**
** - BeatmapPanel()
**   - Default constructor
**
** - BeatmapPanel(texturePanel, textureArrowLeft, textureArrowRight, textureCover, position, res, beatmapConfig, fonts)
**   - Constructor with textures, position, resolution, beatmap config and font
**
** - ~BeatmapPanel()
**   - Destructor
**
** - draw()
**   - Draws the panel
**
** - setText()
**   - Sets the text
**
** - adjust()
**   - Adjusts the panel
**
** - adjustPanel()
**   - Adjusts the panel sprite
**
** - adjustCover()
**   - Adjusts the cover sprite
**
** - adjustGrade()
**   - Adjusts the grade sprite
**
** - adjustDifficulty()
**   - Adjusts the difficulty
**
** - getBeatmapName()
**   - Gets the beatmap name
**
** - getFolderPath()
**   - Gets the beatmap folder path
**
** - handleEvent()
**   - Handles events
**
** Private Attributes:
**
** - textures
** - sprites
** - buttons
** - buttonConfigs
** - texts
** - difficultyUser
** - position
** - size
** - textSize
** - folderPath
** - font
*/

#ifndef BEATMAPPANNEL_HPP_
#define BEATMAPPANNEL_HPP_

#include <functional>
#include <SFML/Graphics.hpp>
#include <string>
#include <iostream>
#include <utility>
#include <cmath>
#include "Beatmap/Beatmap.hpp"
#include "Database/Database.hpp"
#include "GraphicElement/Button.hpp"

class BeatmapPanel {
public:
    BeatmapPanel();
    BeatmapPanel(const sf::Texture& texturePanel, const sf::Texture& textureArrowLeft, const sf::Texture& textureArrowRight,
                 const sf::Texture& textureCover, const sf::Vector2f& position, std::pair<int, int> res, const BeatmapConfig& beatmapConfig, const sf::Font& fonts);
    ~BeatmapPanel();
    void draw(sf::RenderWindow& window);
    void setText(const std::string& text, const sf::Font& font, unsigned int characterSize);
    void adjust(float scale, float opacity, const sf::Vector2f& offset, std::pair<int, int> res);
    void adjustPanel(float scale, float opacity, const sf::Vector2f& offset, std::pair<int, int> res);
    void adjustCover(float scale, float opacity, const sf::Vector2f& offset, std::pair<int, int> res);
    void adjustGrade(float scale, float opacity, const sf::Vector2f& offset, std::pair<int, int> res);
    void adjustDifficulty(int difficulty);
    int getDifficultyUser();
    std::string getBeatmapName() {
        std::string name = texts["title"].getString();
        name.erase(0, 8);
        return name;
    }
    std::string getFolderPath() {
        return folderPath;
    }
    void handleEvent(const sf::Event& event, const sf::RenderWindow& window);
    //void adjustDifficultyUserPlus();
private:
    sf::Texture texture;
    std::map<std::string, sf::Sprite> sprites; // Replace individual sprites with a map
    std::map<string, Button> buttons;
    map<string, ButtonConfig> buttonConfigs;
    //sf::Texture textureCover;
    std::map<std::string, sf::Text> texts; // Map for text elements
    int difficultyUser;
    sf::Vector2f position;
    sf::Vector2f size;
    int textSize;
    std::string folderPath;
    sf::Font _font;
};
#endif /*BEATMAPPANNEL_HPP_*/
