/*
** EPITECH PROJECT, 2023
** Rtype
** File description:
** Button.hpp
*/

/**
** Button
**
** This class represents a clickable button.
**
** Public Methods:
**
** - Button()
**   - Default constructor
**
** - Button(texture, position, onClick, res)
**   - Constructor with texture, position, click handler and resolution
**
** - ~Button()
**   - Destructor
**
** - handleEvent()
**   - Handles mouse events on the button
**
** - draw()
**   - Draws the button
**
** - setText()
**   - Sets the text on the button
**
** Private Attributes:
**
** - texture
**   - Button texture
**
** - sprite
**   - Button sprite
**
** - text
**   - Button text
**
** - onClick
**   - Click handler function
**
** - state
**   - Current state of the button
**
** - position
**   - Position of the button
**
** - size
**   - Size of the button
**
** - type
**   - Type of the button (rectangle, circle, etc)
**
** - lastClickClock
**   - Clock to prevent double clicks
*/

#ifndef BUTTON_HPP_
#define BUTTON_HPP_

#include <functional>
#include <SFML/Graphics.hpp>
#include <string>
#include <iostream>
enum ButtonType {
    RECTANGLE,
    TRIANGLE,
    CIRCLE,
    TEXT
};

enum ButtonState {
    NORMAL,
    HOVER,
    PRESSED,
    DISABLED
};
typedef struct ButtonConfig_s {
    std::string text;
    std::function<void()> onClick;
} ButtonConfig;
class Button {
public:
    Button();
    Button(const sf::Texture& texture, const sf::Vector2f& position, std::function<void()> onClick, std::pair<int, int> res);
    ~Button();

    void handleEvent(const sf::Event& event, const sf::RenderWindow& window);
    void draw(sf::RenderWindow& window);
    void setText(const std::string& text, const sf::Font& font, unsigned int characterSize);

private:
    sf::Texture texture;
    sf::Sprite sprite;
    sf::Text text;
    std::function<void()> onClick;
    ButtonState state;
    sf::Vector2f position;
    sf::Vector2f size;
    ButtonType type;
    sf::Clock lastClickClock = sf::Clock();

};
#endif /*BUTTON_HPP_*/
