/*
** EPITECH PROJECT, 2024
** Rtype
** File description:
** global
*/

/**
** GameState
**
** This file defines an enum for game states.
**
** GameState enum values:
**
** - MENU
**
** - MENUPRELOAD
**
** - LOBBY
**
** - LOBBYLIST
**
** - LOBBYLISTPRELOAD
**
** - LOBBYSCORE
**
** - LOBBYSCOREPRELOAD
**
** - GAME
**
** - WELCOMEMENU
**
** - EXIT
**
** - SKINCHOICE
**
** - ENDT
**
*/


#ifndef GAMESTATE_HPP
#define GAMESTATE_HPP

typedef enum  {
    MENU,
    MENUPRELOAD,
    LOBBY,
    LOBBYLIST,
    LOBBYLISTPRELOAD,
    LOBBYSCORE,
    LOBBYSCOREPRELOAD,
    GAME,
    WELCOMEMENU,
    EXIT,
    SKINCHOICE,
    ENDT
}GameState;


#endif /* !GAMESTATE_HPP */