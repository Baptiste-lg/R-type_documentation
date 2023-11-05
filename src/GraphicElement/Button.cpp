/*
** EPITECH PROJECT, 2023
** Rtype
** File description:
** Button.cpp
*/

/**
** Button.cpp
**
** Implements a clickable button UI element.
**
** Methods:
**
** - Button()
**   Default constructor.
**
** - Button()
**   Constructor taking texture, position, click handler and resolution.
**   Calculates pixel position from normalized input.
**   Sets sprite texture and position.
**
** - ~Button()
**   Destructor.
**
** - handleEvent()
**   Handles click events on the button.
**   Calls onClick handler if button is clicked.
**
** - draw()
**   Draws the button sprite and text.
**
** - setText()
**   Sets the text string, font, and size.
**   Centers text in button.
**
** Members:
**
** - sprite
**   The button sprite.
**
** - text
**   The text displayed on the button.
**
** - onClick
**   The click handler function.
**
** - position
**   The pixel position of the button.
**
** - size
**   The size of the button.
**
** - lastClickClock
**   Tracks time since last click to prevent double clicks.
**
*/

#include "GraphicElement/Button.hpp"

Button::Button() {
}

Button::Button(const sf::Texture& texture, const sf::Vector2f& position, std::function<void()> onClick, std::pair<int, int> res)
        : onClick(onClick)
{
    sprite.setTexture(texture);
    // Convert position from normalized coordinates to pixel coordinates
    sf::Vector2f pixelPosition;
    pixelPosition.x = position.x / 100.0f * res.first;
    pixelPosition.y = position.y / 100.0f * res.second;
    sprite.setPosition(pixelPosition);
    this->position = pixelPosition; // Set the position member variable
    this->size = sf::Vector2f(texture.getSize()); // Set the size member variable based on the texture size

}


Button::~Button() {
}

void Button::handleEvent(const sf::Event& event, const sf::RenderWindow& window) {
    if (event.type == sf::Event::MouseButtonPressed) {
        sf::Vector2i mousePos = sf::Mouse::getPosition(window);
        if (sprite.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
            sf::Time elapsed = lastClickClock.getElapsedTime();
            if (elapsed.asMilliseconds() > 200) {  // 200 milliseconds cooldown
                onClick();
                lastClickClock.restart();
            }
        }
    }
}


void Button::draw(sf::RenderWindow& window) {
    window.draw(sprite);
    window.draw(text);
}


// Button.cpp
void Button::setText(const std::string& text, const sf::Font& font, unsigned int characterSize) {
    this->text.setString(text);
    this->text.setFont(font);
    this->text.setCharacterSize(characterSize);
    // Center the text in the button
    sf::FloatRect textRect = this->text.getLocalBounds();
    this->text.setOrigin(textRect.left + textRect.width/2.0f, textRect.top  + textRect.height/2.0f);
    this->text.setPosition(sf::Vector2f(position.x + size.x/2.0f, position.y + size.y/2.0f));
}
