/*
** EPITECH PROJECT, 2023
** Rtype
** File description:
** RectangleButton.hpp
*/

/**
** RectangleButton
**
** This class represents a rectangular button.
** It inherits from the Button base class.
**
** Public Methods:
**
** - RectangleButton()
**   - Constructor with position, size, text, font, color and text size
**
** - setSize()
**   - Sets the size of the button
**
** Inherited from Button:
**
** - handleEvent()
** - draw()
** - setText()
**
** Private Attributes:
**
** Inherited from Button:
**
** - texture
** - sprite
** - text
** - onClick
** - state
** - position
** - size
** - type
** - lastClickClock
*/

#ifndef RECTANGLEBUTTON_HPP_
	#define RECTANGLEBUTTON_HPP_

#include "Button.hpp"

class RectangleButton : public Button {
public:
    RectangleButton(sf::Vector2f position, sf::Vector2f size,
    const std::string& buttonText, sf::Font& font, sf::Color color, int sizeText);

    void setSize(sf::Vector2f size);
};


#endif /*RECTANGLEBUTTON_HPP_*/