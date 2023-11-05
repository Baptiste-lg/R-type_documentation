/*
** EPITECH PROJECT, 2023
** Rtype
** File description:
** ClientMain.cpp
*/

/**
** ClientMain.cpp
**
** Implements functionality for client-side game entity initialization
** and terminal input handling for the game.
**
** Functions:
**
** - getch()
**   Captures a single character from the terminal input in non-blocking mode,
**   without echoing the input character to the terminal.
**
** - initializeGameEntitiesClient(Registry& registry)
**   Initializes the game entities on the client-side by invoking
**   the spawnClientEntity function with the provided registry.
**
** Dependencies:
**
** - ClientMain.hpp
**   Header file which may contain declarations relevant to client-side operations.
**
** - Spawner.hpp
**   Header file containing the declaration of spawnClientEntity function
**   and possibly other entity spawning related functionalities.
**
*/

#include "ClientMain.hpp"
#include "Spawner.hpp"

char getch() {
    char buf = 0;
    struct termios old = {0};
    fflush(stdout);
    if (tcgetattr(0, &old) < 0) perror("tcsetattr()");
    old.c_lflag &= ~ICANON;
    old.c_lflag &= ~ECHO;
    old.c_cc[VMIN] = 1;
    old.c_cc[VTIME] = 0;
    if (tcsetattr(0, TCSANOW, &old) < 0) perror("tcsetattr ICANON");
    if (read(0, &buf, 1) < 0) perror("read()");
    old.c_lflag |= ICANON;
    old.c_lflag |= ECHO;
    if (tcsetattr(0, TCSADRAIN, &old) < 0) perror("tcsetattr ~ICANON");
    return buf;
}

void initializeGameEntitiesClient(Registry& registry)
{
    //spawn the client entities
    spawnClientEntity(registry);
}
