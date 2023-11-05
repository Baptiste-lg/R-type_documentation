/*
** EPITECH PROJECT, 2024
** Rtype
** File description:
** Event
*/

/**
 * Event.hpp defines the event system used for communication between systems.
 *
 * Events represent things happening in the game, like player actions or state changes.
 * Systems can subscribe to event types to be notified when they occur.
 *
 * Usage:
 * - Define events as an enum of event types.
 * - Create Event struct with type and data.
 * - Fire events on the EventBus.
 * - Systems subscribe to event types.
 * - EventBus invokes all listeners for an event type.
 *
 * EventBus:
 * - Holds listeners in a map of vectors by event type.
 * - addListener() subscribes a listener function.
 * - fireEvent() invokes all listeners for that event type.
 *
 * Events:
 * - type: The event type enum.
 * - content: Additional text data.
 * - clientID: ID of the client that triggered the event.
 *
 * Listeners:
 * - std::function callback invoked when an event fires.
 *
 * This allows decoupled communication between systems.
*/

#ifndef EVENT_HPP_
#define EVENT_HPP_

#include <functional>
#include <iostream>
#include <netinet/in.h>


enum EventType {
    LocalPlayerMoved,
    PlayerAdded,
    PlayerDied,
    ItemPicked,
    PlayerJoined,
    PlayerLeft,
    PlayerMoved,
    PlayerFired,
    PlayerHit,
    PlayerRespawned,
    PlayerScored,
    PlayerWon,
    PlayerLost,
    PlayerDisconnected,
    PlayerReconnected,
    PlayerRejoined,
    PlayerLeftLobby,
    PlayerKicked,
    PlayerBanned,
    PlayerUnbanned,
    EndOfSong,
    playerEndOfSong,
    playerEndScore,
    //MENU_EVENT
    IndexMoved,
    GameStarted,
    GameEnded,
    GamePaused,
    GameResumed,
    GameRestarted,
    GameExited,
    LobbyCreated,
    LobbyJoined,
    LobbyLeft,
    AskLobbyLeft,
    LobbyKicked,
    LobbyBanned,
    LobbyUnbanned,
    AskLobbyDestroyed,
    LobbyDestroyed,
    LobbyUpdated,
    LobbyMessageSent,
    LobbySelectionCreated,
    skinButtonClicled,
    SkinSelected,
    getLobbyList,
    getLobbyInfo,
    ClientTryConnect,
    AskDataCreationLobby,
    AskDataListLobby,
    LobbyJoinRequested,
    PlayerJoinedLobby,
    BackToMenu,
    SaveSkin,
    PlayerReady,
    LobbyPlayerReady,
    HeartBeat,
    FFTStart,
    //music transfer
    WavDataSent,
    WavDataRequested,
    WavDataAsked,
    TCPInit,
    UDPInit,
    FFTEnd,
    FFTPlay,
    AskGameSceneLoad
    // add more event types
};

const std::unordered_map<std::string, EventType> eventIdentifierMap = {
        {"PD", PlayerDied},
        {"PA", PlayerAdded},
        {"IP", ItemPicked},
        {"PJ", PlayerJoined},
        {"PL", PlayerLeft},
        {"PM", PlayerMoved},
        {"PF", PlayerFired},
        {"PH", PlayerHit},
        {"PR", PlayerRespawned},
        {"PS", PlayerScored},
        {"PW", PlayerWon},
        {"PL", PlayerLost},
        {"PDC", PlayerDisconnected},
        {"PRC", PlayerReconnected},
        {"PRJ", PlayerRejoined},
        {"PK", PlayerKicked},
        {"PB", PlayerBanned},
        {"PU", PlayerUnbanned},
        {"EOS", EndOfSong},
        {"PEOS", playerEndOfSong},
        {"PES", playerEndScore},
        //MENU_EVENT
        {"IM", IndexMoved},
        {"GS", GameStarted},
        {"FFT", FFTStart},
        {"GE", GameEnded},
        {"GP", GamePaused},
        {"GR", GameResumed},
        {"GRS", GameRestarted},
        {"GX", GameExited},
        {"ADCL", AskDataCreationLobby},
        {"ADLL", AskDataListLobby},
        {"LC", LobbyCreated},
        {"LJ", LobbyJoined},
        {"LL", LobbyLeft},
        {"PLL", PlayerLeftLobby},
        {"ALL", AskLobbyLeft},
        {"ALD", AskLobbyDestroyed},
        {"LK", LobbyKicked},
        {"LB", LobbyBanned},
        {"LU", LobbyUnbanned},
        {"LD", LobbyDestroyed},
        {"LU", LobbyUpdated},
        {"LMS", LobbyMessageSent},
        {"LSC", LobbySelectionCreated},
        {"GLL", getLobbyList},
        {"GLI", getLobbyInfo},
        {"CTC", ClientTryConnect},
        {"LJR", LobbyJoinRequested},
        {"BTM", BackToMenu},
        {"PLR", PlayerReady},
        {"PJL", PlayerJoinedLobby},
        {"LPR", LobbyPlayerReady},
        {"HB", HeartBeat},
        //music transfer
        {"WDS", WavDataSent},
        {"WDR", WavDataRequested},
        {"WDA", WavDataAsked},
        {"TCP", TCPInit},
        {"UDP", UDPInit}

};
struct Event {
    EventType type;
    std::string content; // event-specific data
    int clientID;
    std::vector<uint8_t> wavData;  // Add this line
    size_t TCPDataSize;  // Add this line
    int TCPSocket;
    struct sockaddr_in UDPAddr;
    // add more event-specific data
};

using Listener = std::function<void(const Event &)>;

class EventBus {
private:
    std::unordered_map<EventType, std::vector<Listener>> listeners;
public:
    void addListener(EventType type, Listener listener) {
        listeners[type].push_back(listener);
    }

    void fireEvent(const Event &event) {
        for (const auto &listener: listeners[event.type]) {
            listener(event);
        }
    }

};

EventType parseEventType(const std::string &eventIdentifier);

#endif /* !EVENT_HPP_ */