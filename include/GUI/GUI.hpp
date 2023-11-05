/*
** EPITECH PROJECT, 2023
** Rtype
** File description:
** GUI.hpp
*/

/**
** GUI
**
** This class represents the graphical user interface.
**
** Public Methods:
**
** - GUI()
**   - Constructor that takes a reference to the render window
**
** - ~GUI()
**   - Destructor
**
** - load()
**   - Loads the GUI based on a GUIConfig
**
** - loadBeatmapPanel()
**   - Loads the beatmap selection panel
**
** - loadingScreenCreate()
**   - Creates the loading screen
**
** - updateLoadingScreen()
**   - Updates the loading screen progress
**
** - resetLoadingScreen()
**   - Resets the loading screen
**
** - reloadLoadingScreen()
**   - Reloads the loading screen resources
**
** - displayLoadingScreen()
**   - Displays the loading screen
**
** - createMenu()
**   - Creates the main menu
**
** - updateMenu()
**   - Updates the main menu
**
** - loop()
**   - Main GUI loop
**
** - createGame()
**   - Creates the game GUI
**
** - updateGame()
**   - Updates the game GUI
**
** - drawGame()
**   - Draws the game GUI
**
** - executeLoadingScreenTasks()
**   - Executes loading screen tasks
**
** - setActive()
**   - Sets the window active state
**
** - getWindow()
**   - Gets the render window
**
** - getEvent()
**   - Gets the SFML event
**
** Private Attributes:
**
** - window
**   - Reference to the render window
**
** - event
**   - SFML event
**
** - mousePos
**   - Mouse position
**
** - mousePosF
**   - Mouse position as float
**
** - loadingScreen
**   - Loading screen object
**
** - menu
**   - Main menu object
*/

#ifndef GUI_HPP_
#define GUI_HPP_
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>
#include <SFML/Audio.hpp>
#include <iostream>
#include <string>
#include <vector>
#include <functional>
#include <thread>
#include "GUI/GUIConfig.hpp"
#include "GraphicElement/LoadingScreen.hpp"
#include "Database/Database.hpp"
#include "Menu/Menu.hpp"

class Core;



class GUI {
	public:
		GUI(sf::RenderWindow& window) : window(window) {};
		~GUI();
        void load(const GUIConfig& config);
        void loadBeatmapPanel(Database& database);
        void loadingScreenCreate();
        void updateLoadingScreen(int pourcent);
        void resetLoadingScreen();
        void reloadLoadingScreen();
        void displayLoadingScreen();
        void createMenu(Database& database);
        void updateMenu();
        void loop(int state);
        void createGame();
        void updateGame();
        void drawGame();
        void executeLoadingScreenTasks(const std::vector<std::function<void(int&)>>& tasks);
        void setActive(bool active) {
            window.setActive(active);
        }
        sf::RenderWindow& getWindow() {
            return window;
        }
        sf::Event& getEvent() {
            return event;
        }
    Menu menu;

protected:
        sf::RenderWindow& window;
        sf::Event event;
        sf::Vector2i mousePos;
        sf::Vector2f mousePosF;
        LoadingScreen loadingScreen;
private:
};

#endif /*GUI_HPP_*/