/*
** EPITECH PROJECT, 2024
** ECS
** File description:
** Event
*/

#ifndef EVENT_HPP_
#define EVENT_HPP_

#include <functional>
#include <iostream>


enum EventType {
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
    PlayerKicked,
    PlayerBanned,
    PlayerUnbanned
    // add more event types
};

const std::unordered_map<std::string, EventType> eventIdentifierMap = {
        {"PD", PlayerDied},
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
        {"PU", PlayerUnbanned}
};
struct Event {
    EventType type;
    std::string content; // event-specific data
    int clientID;
    // add more event-specific data
};

using Listener = std::function<void(const Event&)>;

class EventBus {
private:
    std::unordered_map<EventType, std::vector<Listener>> listeners;
public:
    void addListener(EventType type, Listener listener) {
        std::cout << "Listener added for event type: " << static_cast<int>(type) << std::endl;
        listeners[type].push_back(listener);
    }

    void fireEvent(const Event& event) {
        std::cout << "Firing event of type: " << static_cast<int>(event.type) << std::endl;
        for (const auto& listener : listeners[event.type]) {
            listener(event);
        }
    }

};

EventType parseEventType(const std::string& eventIdentifier);

#endif /* !EVENT_HPP_ */