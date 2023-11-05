/*
** EPITECH PROJECT, 2024
** Rtype
** File description:
** System.hpp
*/

/**
* System.hpp
*
* Defines enums for global player state.
*
* Enums:
*
* - PlayerState
*   Overall player state.
*   - InMenu
*   - InGame
*   - InLobby
*
* - PlayerSubState
*   Detailed player sub-state when in game.
*   - isLoading
*   - isDownloading
*   - isWaiting
*   - isReady
*   - isPlaying
*   - isDead
*   - isWin
*   - isLose
*   - isDisconnected
*   - isReconnecting
*   - isReconnected
*   - isReconnectingFailed
*
*/

#ifndef SYSTEM_HPP_
#define SYSTEM_HPP_
#include <SFML/Graphics.hpp>

enum class PlayerState {
    InMenu,
    InGame,
    InLobby
};
enum class PlayerSubState {
    isLoading,
    isDownloading,
    isWaiting,
    isReady,
    isPlaying,
    isDead,
    isWin,
    isLose,
    isDisconnected,
    isReconnecting,
    isReconnected,
    isReconnectingFailed
};

#endif /* !SYSTEM_HPP_ */

