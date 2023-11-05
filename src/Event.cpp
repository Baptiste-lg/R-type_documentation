/*
** EPITECH PROJECT, 2024
** Rtype
** File description:
** Event.cpp
*/

/**
** Event.cpp
**
** This file contains the implementation for parsing event types from string identifiers.
** It defines a function `parseEventType` which maps string identifiers to their corresponding
** event types as defined in the EventType enumeration.
**
** Functions:
**
** - parseEventType(const std::string& eventIdentifier)
**   This function takes a string identifier as an argument and returns the corresponding
**   EventType enumeration value. It makes use of the `eventIdentifierMap` to look up
**   the corresponding event type. If the identifier is not found in the map, it will throw
**   an exception.
**
** Dependencies:
**
** - "Event.hpp" for EventType enumeration and eventIdentifierMap declaration.
** - <string> for std::string.
**
*/

#include "Event.hpp"

EventType parseEventType(const std::string& eventIdentifier){
    return eventIdentifierMap.at(eventIdentifier);
}