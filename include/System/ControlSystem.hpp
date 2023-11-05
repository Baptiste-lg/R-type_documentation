/*
** EPITECH PROJECT, 2024
** Rtype
** File description:
** SYSTEM
*/

/**
 * ControlSystem class
 *
 * Inherits from ISystem interface. Handles controlling entities with Controllable,
 * Position, Velocity, and GameConfig components.
 *
 * deltaTime: Reference to delta time value for frame rate independence.
 * registry: Reference to global ECS registry.
 * eventQueue: Queue of input events mapped to entities.
 *
 * Constructor:
 * - Initializes event listener for player input events.
 *
 * operator():
 * - Entry point for system.
 * - Checks for Controllable, Velocity, and GameConfig components.
 * - Applies player input events from queue to entity velocity.
 * - Updates entity position based on velocity.
*/

#ifndef CONTROLSYSTEM_HPP_
#define CONTROLSYSTEM_HPP_
#include "../include/System/ISystem.hpp"
#include "../include/System/System.hpp"
#include "../include/TextureManager.hpp"
#include "../include/Registry.hpp"
#include "../include/Spawner.hpp"
class ControlSystem : public ISystem<Position, Velocity, Controllable>
        {
private:
    float &deltaTime;
    Registry &registry;  // Add this line
    std::unordered_map<entity_t, std::queue<Event>> eventQueue;  // Queue to hold events for each entity
    std::vector<Event> eventsToSend;
    std::unordered_map<entity_t, std::unordered_set<char>> inputStates;
    std::unordered_map<entity_t, std::unordered_set<char>> lastInputStates;
public:
    std::string const getSystemName() override {
        return "ControlSystem";
    }
    ControlSystem(float &deltaTime, Registry &registry) : deltaTime(deltaTime), registry(registry) {
        registry.eventBus.addListener(PlayerMoved, [this, &registry](const Event &event) {
            //will reveive event from server system PM:clientID:content
            std::vector<std::string> eventContent = tokenize(event.content, ":");
            entity_t entity = registry.clientToEntityMap[std::stoi(eventContent[0])];
            if (entity != 0) {
                applyInputState(entity, eventContent[1]);
            }
        });
    }  // Modify this line
    void applyInputState(entity_t entity, const std::string& serializedState) {
        // Update the input states based on the serialized state.
        std::unordered_set<char> newInputs(serializedState.begin(), serializedState.end());
        inputStates[entity] = newInputs; // Replace the current state with the new state.
        // we juste need to check for when it will receive the event
        // The actual application of this state to the entity's velocity will be done in the operator() method.
    }

    void operator()(entity_t entity, sparse_array<Position> &positions, sparse_array<Velocity> &velocities, sparse_array<Controllable> &controllables) override
    {
        auto controllableOpt = controllables[entity];
        auto velOpt = velocities[entity];
        auto &gameConfigOpt = registry.get_components<GameConfig>()[registry.stateEntity];

        if (controllableOpt.has_value() && velOpt.has_value() && gameConfigOpt.has_value()) {
            auto &controllable = *controllables[entity];
            if (!controllable.isLocal && velOpt.has_value() && gameConfigOpt.has_value()) {
                auto &vel = *velocities[entity];
                auto &config = *gameConfigOpt;

                    // Apply continuous input based on the inputStates
                    if (inputStates.find(entity) != inputStates.end()) {
                        const auto& states = inputStates[entity];
                        vel.x = 0.0f;
                        vel.y = 0.0f;
                        if (states.count('Q')) vel.x = -1.0f * config.playerSpeed * deltaTime;
                        if (states.count('D')) vel.x = 1.0f * config.playerSpeed * deltaTime;
                        if (states.count('Z')) vel.y = -1.0f * config.playerSpeed * deltaTime;
                        if (states.count('S')) vel.y = 1.0f * config.playerSpeed * deltaTime;
                    }

            } else if (velOpt.has_value() && gameConfigOpt.has_value() && controllable.isLocal) {
                auto &vel = *velocities[entity];
                auto &config = *gameConfigOpt;
                //check sf event
                //check sf event
                // Use the global config
                vel.x = 0.0f;
                vel.y = 0.0f;
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::Z)) {
                    vel.y = -1.0 * config.playerSpeed * deltaTime;
                }
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
                    vel.y = 1.0 * config.playerSpeed * deltaTime;
                }
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::Q)) {
                    vel.x = -1.0f * config.playerSpeed * deltaTime;
                }
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
                    vel.x = 1.0f * config.playerSpeed * deltaTime;
                }
                updateInputState(entity, 'S', sf::Keyboard::isKeyPressed(sf::Keyboard::S));
                updateInputState(entity, 'Z', sf::Keyboard::isKeyPressed(sf::Keyboard::Z));
                updateInputState(entity, 'Q', sf::Keyboard::isKeyPressed(sf::Keyboard::Q));
                updateInputState(entity, 'D', sf::Keyboard::isKeyPressed(sf::Keyboard::D));
                static bool lastKeyState = false;
                    //fire event of end game
                if (!lastKeyState && sf::Keyboard::isKeyPressed(sf::Keyboard::P)) {
                    Event NewEvent;
                    NewEvent.type= EventType::EndOfSong;
                    registry.eventBus.fireEvent(NewEvent);
                    lastKeyState = true;
                }


            //will triger event to send to client system that will send to server
            }
        }
    }
    std::string serializeInputStates(entity_t entity, const std::unordered_set<char>& states) {
        std::string result;
        for (char input : states) {
            result += input;
        }
        return result;
    }
    void updateInputState(entity_t entity, char input, bool isPressed) {
        // Check if the input state needs to be updated
        bool currentStateContainsInput = inputStates[entity].count(input) > 0;

        // If the key is pressed and not already in the state, add it
        if (isPressed && !currentStateContainsInput) {
            inputStates[entity].insert(input);
        }
            // If the key is released and is in the state, remove it
        else if (!isPressed && currentStateContainsInput) {
            inputStates[entity].erase(input);
        }
    }

    void PostUpdate() override {
        // Iterate over inputStates to find changes
        for (const auto& [entity, currentStates] : inputStates) {
            // Get the controllable component to check if the entity is local
            auto controllableOpt = registry.get_components<Controllable>()[entity];

            // Check if the entity is local and the input state has changed
            if (controllableOpt.has_value() && controllableOpt->isLocal) {
                const auto& lastStates = lastInputStates[entity];
                if (currentStates != lastStates) {
                    std::string message = serializeInputStates(entity, currentStates);
                    // Send the message for the local player only
                    registry.eventBus.fireEvent(Event{LocalPlayerMoved, message});
                }
            }
        }

        // Update lastInputStates to be used in the next frame
        lastInputStates = inputStates;
    }


};

#endif /* !CONTROLSYSTEM_HPP_ */