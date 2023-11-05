/*
** EPITECH PROJECT, 2023
** Rtype
** File description:
** Menu.cpp
*/

/**
* Menu.cpp
*
* Implements the main menu UI.
*
* Methods:
*
* - Menu()
*   Constructor.
*
* - ~Menu()
*   Destructor.
*
* - loadTextures()
*   Loads menu textures from files.
*
* - load()
*   Loads menu buttons, fonts, backgrounds.
*   Calls loadBeatmaps().
*
* - loadBeatmaps()
*   Loads BeatmapPanel objects for each beatmap.
*
* - update()
*   Handles events and updates selected panel.
*   Returns current menu state.
*
* - draw()
*   Draws menu elements on render window.
*
* - slideLeft()
*   Slides selection left by one panel.
*
* - slideRight()
*   Slides selection right by one panel.
*
* - onAnalyzeButtonClicked()
*   Called when Analyze button clicked.
*   Prints message and loads music.
*
* - getActualSongName()
*   Gets folder path of selected beatmap.
*
* - setDifficulty()
*   Sets difficulty on selected panel.
*
* - getDifficulty()
*   Gets difficulty from selected panel.
*
* Members:
*
* - textures
*   Loaded texture map.
*
* - fonts
*   Loaded font map.
*
* - buttons
*   Menu buttons.
*
* - sprites
*   Menu sprites.
*
* - beatmapPanel
*   Vector of BeatmapPanel objects.
*
* - selectedPanelIndex
*   Index of currently selected panel.
*
* - state
*   Current menu state.
*
* - _res
*   Window resolution.
*
*/

#include "Menu/Menu.hpp"

Menu::Menu()
{

}

Menu::~Menu()
{
}

void Menu::loadTextures(Database& database)
{
    // Load the background texture
    if (!textures["background"].loadFromFile("asset/UI/Background.jpg")) {
        std::cout << "Error loading texture : " << "asset/UI/Background.jpg" << std::endl;
        // Handle error...
    }
    // Load button textures
    for (int i = 1; i <= 6; ++i) {
        std::string texturePath = "asset/UI/Btn0" + std::to_string(i) + ".png";
        if (!textures["Btn0" + std::to_string(i)].loadFromFile(texturePath)) {
            std::cout << "Error loading texture " << texturePath << std::endl;
            // Handle error...
        }
    }

    // Load the panel texture
    if (!textures["MainPanel01"].loadFromFile("asset/UI/MainPanel01.png")) {
        std::cout << "Error loading texture : " << "asset/UI/MainPanel01.png" << std::endl;
        // Handle error...
    }
    // Load arrow textures

    if (!textures["ArrowsRight"].loadFromFile("asset/UI/ArrowsRight.png")) {
        std::cout << "Error loading texture : " << "asset/UI/ArrowsRight.png" << std::endl;
        // Handle error...
    }
    if (!textures["ArrowsLeft"].loadFromFile("asset/UI/ArrowsLeft.png")) {
        std::cout << "Error loading texture : " << "asset/UI/ArrowsLeft.png" << std::endl;
        // Handle error...
    }
    // Load the cover texture
    for (int i = 0; i < database.getNbBeatmaps(); ++i) {
        BeatmapConfig beatmap = database.getBeatmap(i);
        if (beatmap.getFolderPath() == "") {
            continue;
        }
        std::string coverPath = "asset/Beatmaps/" + beatmap.getFolderPath() + "/Cover.jpg";
        if (!textures[beatmap.getFolderPath()].loadFromFile(coverPath)) {
            std::cout << "Error loading texture : " << coverPath << std::endl;
            // Handle error...
        }
    }
}



// Menu.cpp
void Menu::load(const std::pair<int, int>& res, Database& database)
{
    buttonConfigs["Left"] = {"Left", [this](){ this->slideLeft(); }};
    buttonConfigs["Right"] = {"Right", [this](){ this->slideRight(); }};
    buttonConfigs["Exit"] = {"Exit", [](){ std::cout << "Exiting..." << std::endl; /* Add exit logic here */ }};
    buttonConfigs["Analyse"] = {"Analyse", [this](){
        this->onAnalyzeButtonClicked(getActualSongName());
    }};
    buttonConfigs["Play"] = {"Play", [this](){std::cout<< "Play..." << std::endl; this->state = PLAYWAITING;}};
    buttonConfigs["Options"] = {"Options", [](){std::cout<< "Options..." << std::endl;}};
    // Load the background image
    loadTextures(database);
    sf::Sprite background(textures["background"]);
    sprites["background"] = background;

    // Load the font
    if (!fonts["sansation"].loadFromFile("asset/Fonts/sansation.ttf")) {
        std::cout << "Error loading font" << std::endl;
        // Handle error...
    }
    int i = 1;
    for (const auto& [key, config] : buttonConfigs) {
        std::string buttonName = "button" + std::to_string(i);
        buttons.emplace(buttonName, Button(textures["Btn0" + std::to_string(i)],
                                           sf::Vector2f(10 + i * 10, 80),
                                           config.onClick, res));
        buttons[buttonName].setText(config.text, fonts["sansation"], 30);
        ++i;
    }


    // Load the bottom buttons

    this->_res = res;
    loadBeatmaps(database);
}

void Menu::loadBeatmaps(Database& database) {
    for (int i = 0; i < database.getNbBeatmaps(); ++i) {
        BeatmapConfig beatmap = database.getBeatmap(i);
        if (beatmap.getFolderPath() == "") {
            continue;
        }
        
        // Create a BeatmapPanel object and add it to the beatmapPanel vector
        beatmapPanel.push_back(std::make_unique<BeatmapPanel>(textures["MainPanel01"], textures["ArrowsLeft"], textures["ArrowsRight"],
                                                               textures[beatmap.getFolderPath()],
                                                               sf::Vector2f(5, 5), _res, beatmap, fonts["sansation"]));
    }
}


int Menu::update(const sf::Event& event, const sf::RenderWindow& window)
{
    for (auto& button : buttons) {
        button.second.handleEvent(event, window);
    }
    int selectedPanel = selectedPanelIndex;

    //we need to only check the panel that is selected
    int y = 0;
    for (auto& panel : beatmapPanel) {
        if (y == selectedPanel) {
            panel->handleEvent(event, window);
        }
        ++y;
    }
    // Determine the selected panel
    // Adjust the panels
    for (int i = 0; i < beatmapPanel.size(); ++i) {
        if (i < selectedPanel) {
            beatmapPanel[i]->adjust(0.5f, 128, sf::Vector2f(2, 4), _res);
        } else if (i > selectedPanel) {
            beatmapPanel[i]->adjust(0.5f, 128, sf::Vector2f(8, 4), _res);
        } else {
            beatmapPanel[i]->adjust(0.8f, 255, sf::Vector2f(5, 5), _res);
        }
    }
    return this->state;
}


void Menu::draw(sf::RenderWindow& window)
{
    window.draw(sprites["background"]);
    for (auto& button : buttons) {
        button.second.draw(window);
    }

    // Draw the panels in reverse order
    for (int i = beatmapPanel.size() - 1; i >= 0; --i) {
        beatmapPanel[i]->draw(window);
    }
}

void Menu::slideLeft() {
    if (selectedPanelIndex > 0) {
        selectedPanelIndex--;
    }
    // You can add wrap-around logic here if you want the selection to loop back to the last panel when at the first panel.
}

void Menu::slideRight() {
    if (selectedPanelIndex < beatmapPanel.size() - 1) {
        selectedPanelIndex++;
    }
    // You can add wrap-around logic here if you want the selection to loop back to the first panel when at the last panel.
}


void Menu::onAnalyzeButtonClicked(const std::string& beatmapPath)
{
    //fft.loadMusic(beatmapPath); // Assuming fftMenu is an instance of FFT_Menu in the Menu class
}

std::string Menu::getActualSongName() {

    std::string beatmapPath = "asset/Beatmaps/" +
     beatmapPanel[selectedPanelIndex]->getFolderPath();
    return beatmapPath;
}

void Menu::setDifficulty(int difficulty) {
    this->beatmapPanel[selectedPanelIndex]->adjustDifficulty(difficulty);
}

int Menu::getDifficulty() {
    return this->beatmapPanel[selectedPanelIndex]->getDifficultyUser();
}