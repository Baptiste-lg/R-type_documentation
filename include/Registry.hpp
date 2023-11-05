/*
** EPITECH PROJECT, 2024
** Rtype
** File description:
** REGISTRY
*/

/**
** Registry is the central class for entity management in the ECS.
** It handles entity creation/destruction and stores component arrays.
**
** Key Responsibilities:
** - Entity ID management
** - Component array storage/access
** - System registration and execution
** - Entity creation/destruction
**
** Usage:
** - Create a Registry instance
** - Register component arrays with register_component()
** - Register systems with add_system()
** - Spawn entities and add components with add_component()
** - Run systems each frame via run_systems()
** - Clean up dead entities with delete_queued_entities()
**
** Public Interface:
**
** - spawn_entity()
**   Creates a new active entity and returns the id.
**
** - spawnInactivedEntity()
**   Creates a new inactive entity and returns the id.
**
** - kill_entity()
**   Destroys an active entity.
**
** - activate_entity()
**   Activates an inactive entity.
**
** - deactivate_entity()
**   Deactivates an active entity.
**
** - entity_from_index()
**   Gets entity id from index.
**
** - add_component()
**   Adds a component to an entity.
**
** - emplace_component()
**   Constructs and adds a component to an entity.
**
** - remove_component()
**   Removes a component from an entity.
**
** - register_component()
**   Registers a new component array.
**
** - get_components()
**   Gets a component array.
**
** - add_system()
**   Registers a new system.
**
** - run_systems()
**   Runs all registered systems.
**
** - queue_for_deletion()
**   Queues an entity for deletion.
**
** - delete_queued_entities()
**   Deletes queued entities.
**
** - queue_for_creation()
**   Queues an entity for creation.
**
** - create_queued_entities()
**   Creates queued entities.
*/

#ifndef REGISTRY_HPP_
#define REGISTRY_HPP_
#include <unordered_map>
#include <typeindex>
#include <any>
#include <functional>
#include "GameState.hpp"
#include "SparseArray.hpp"
#include "Event.hpp"
#include "../include/System/ISystem.hpp"
#include "Component.hpp"
#include "ThreadPool.hpp"
#include <iostream>
#include <unordered_set>
#include <vector>
#include <memory>
#include <utility>
#include <functional>
#include <chrono>  // for profiling
#include <thread>  // for multi-threading
#include <algorithm> // for std::for_each
#include <future>  // for std::async
#include <shared_mutex> // C++17

using entity_t = std::size_t;
class Registry {
public:
    Registry()  {  // Initialize thread pool with 4 threads
        gameStates = std::make_tuple(true, GameState::ENDT, GameState::MENU, GameState::ENDT);
    }

    entity_t spawn_entity() {
        entity_t e;
        if (!_free_ids.empty()) {
            e = _free_ids.back();
            _free_ids.pop_back();
        } else {
            e = _next_id++;
        }
        _active_entities.insert(e);
        return e;
    }

    entity_t spawnInactivedEntity() {
        entity_t e;
        if (!_free_ids.empty()) {
            e = _free_ids.back();
            _free_ids.pop_back();
        } else {
            e = _next_id++;
        }
        _inactive_entities.insert(e);  // <-- change here
        return e;
    }

    void kill_entity(entity_t const& e) {
        if (e == stateEntity) {
            return;
        }
        _free_ids.push_back(e);
        for (auto& [type, eraser] : _eraser_functions) {
            eraser(e);
        }
        _active_entities.erase(e);
    }

    void activate_entity(entity_t const& e) {
        auto it = _inactive_entities.find(e);
        if (it != _inactive_entities.end()) {
            _inactive_entities.erase(it);
            // Call the constructor lambda to add the components
            if (_entity_constructors.find(e) != _entity_constructors.end()) {
                _entity_constructors[e](*this, e);
                _entity_constructors.erase(e);
            }
            _active_entities.insert(e);  // Move this here
        }
    }

    void deactivate_entity(entity_t const& e) {
        auto it = _active_entities.find(e);
        if (it != _active_entities.end()) {
            _active_entities.erase(it);
            _inactive_entities.insert(e);
        }
    }

    entity_t entity_from_index(std::size_t idx) {
        return idx;
    }

    template <typename Component>
    typename sparse_array<Component>::reference_type add_component(entity_t const& to, Component&& c) {
        auto& componentArray = get_components<Component>();
        return componentArray.insert_at(to, std::forward<Component>(c));
    }

    template <typename Component, typename... Params>
    typename sparse_array<Component>::reference_type emplace_component(entity_t const& to, Params&&... p) {
        auto& componentArray = get_components<Component>();
        return componentArray.emplace(to, std::forward<Params>(p)...);
    }

    template <typename Component>
    void remove_component(entity_t const& from) {
        auto& componentArray = get_components<Component>();
        componentArray.erase(from);
    }

    template <class Component>
    sparse_array<Component>& register_component() {
        std::type_index typeIndex(typeid(Component));
        _components_arrays[typeIndex] = std::make_any<sparse_array<Component>>();
        _eraser_functions[typeid(Component)] = [this](entity_t e) {
            auto& array = std::any_cast<sparse_array<Component>&>(_components_arrays[typeid(Component)]);
            array.erase(e);
        };
        return std::any_cast<sparse_array<Component>&>(_components_arrays[typeIndex]);
    }

    template <class Component>
    sparse_array<Component>& get_components() {
        std::type_index typeIndex(typeid(Component));
        if (_components_arrays.find(typeIndex) == _components_arrays.end()) {
            throw std::runtime_error("Component not registered");
        }
        return std::any_cast<sparse_array<Component>&>(_components_arrays[typeIndex]);
    }

    template <class Component>
    const sparse_array<Component>& get_components() const {
        std::type_index typeIndex(typeid(Component));
        return std::any_cast<const sparse_array<Component>&>(_components_arrays.at(typeIndex));
    }

    template <class SystemType, class... Components>
    void add_system(std::unique_ptr<SystemType> sys) {
        auto shared_sys = std::shared_ptr<SystemType>(std::move(sys));
        _system_instances.push_back(std::static_pointer_cast<ISystemBase>(shared_sys));
        auto wrapper = [this, shared_sys](entity_t e) {
            if ((get_components<Components>().contains(e) && ...)) {
                shared_sys->operator()(e, get_components<Components>()...);
            }
        };
        _systems.push_back(wrapper);
        _system_instance_ids[shared_sys->getSystemName()] = std::static_pointer_cast<ISystemBase>(shared_sys);
    }
    void call_post_update_for_system(const std::string& systemID) {
        auto it = _system_instance_ids.find(systemID);
        if (it != _system_instance_ids.end()) {
            it->second->PostUpdate();
        } else {
            std::cerr << "System with ID '" << systemID << "' not found for PostUpdate call." << std::endl;
        }
    }

    void run_systems() {
        // Profiling: Start time

        for (const auto& system : _systems) {
            std::string name = system.target_type().name();
            //check if the system is the DrawingSystem if so skip it
            if (name.find("DrawSystem") != std::string::npos) {
                continue;
            }
            for (entity_t e : _active_entities) {
                if (stateChanged) {
                    break;
                }
                system(e);
            }
            if (stateChanged) {
                stateChanged = false;
            }
            if (postUpdateBreaker) {
                postUpdateBreaker = false;
                break;
            }
            for (const auto& instance : _system_instance_ids) {
                if (instance.first == "DrawSystem") {
                    continue;
                }
                instance.second->PostUpdate();
            }
        }
    }

    void run_draw_system(){
        //use the name of the system  and check for "DrawSystem"
        for (const auto& system : _systems) {
            std::string name = system.target_type().name();
            //check if the system is the DrawingSystem if so skip it
            if (name.find("DrawSystem") != std::string::npos) {
                for (entity_t e : _active_entities) {
                    system(e);
                }
                call_post_update_for_system("DrawSystem");
                break;
            } else {
                continue;
            }
        }
    }

    void queue_for_deletion(entity_t const& e) {
        _entities_to_delete.push_back(e);
    }

void delete_queued_entities() {
    for (auto e : _entities_to_delete) {
        // Now delete the entity
        kill_entity(e);
    }
    _entities_to_delete.clear();
}

    entity_t queue_for_creation() {
        entity_t e;
        if (!_free_ids.empty()) {
            e = _free_ids.back();
            _free_ids.pop_back();
        } else {
            e = _next_id++;
        }
        _inactive_entities.insert(e);  // Add to inactive entities
        return e;
    }
    entity_t queue_for_creation(std::function<void(Registry&, entity_t)> constructor) {
        entity_t e = queue_for_creation();  // the original function
        _entity_constructors[e] = constructor;
        return e;
    }

    void create_queued_entities() {
        _newly_created_entities.clear();  // Clear the list of newly created entities
        for (auto e : _entities_to_create) {
            _newly_created_entities.push_back(e);  // Add to the list of newly created entities
        }
        _entities_to_create.clear();
    }

    std::vector<entity_t> get_newly_created_entities() const {
        return _newly_created_entities;  // Return the list of newly created entities
    }

    void AddComponentsToNewEntities() {
        for (auto e : _newly_created_entities) {
            for (const auto& add_components : component_adders) {
                add_components(e, *this);
            }
            _active_entities.insert(e);
        }
        component_adders.clear();  // Clear the component_adders vector
        _newly_created_entities.clear();  // Clear the list of newly created entities
    }
    void printFreeIds() {
    }
    void addComponentAdder(std::function<void(entity_t, Registry&)> adder) {
        component_adders.push_back(adder);
    }
    std::tuple<bool, GameState, GameState, GameState> gameStates;
    void setPath(const std::string& path) {
        path_to_music = path;
    }
    std::string getPath() {
        return path_to_music;
    }

    void clearInactiveEntities() {
        for (auto e : _inactive_entities) {
            _free_ids.push_back(e);
            for (auto& [type, eraser] : _eraser_functions) {
                eraser(e);
            }
        }
        _inactive_entities.clear();
        clearEntityConstructors();

    }
    void clearActiveEntities() {
        _active_entities.clear();
    }
    void clearFreeIds() {
        _free_ids.clear();
    }
    void clearEntityConstructors() {
        _entity_constructors.clear();
    }
    void clearEntityToCreate() {
        _entities_to_create.clear();
    }

    void clearForNewSong() {
        for (auto e : _active_entities) {
            _free_ids.push_back(e);
        }
        for (auto e : _inactive_entities) {
            _free_ids.push_back(e);
        }
        clearInactiveEntities();
        clearEntityConstructors();
        clearEntityToCreate();
    }
    void clearGameState() {
        // Your logic for clearing the game state, similar to clearForNewSong but for active entities as well.
        std::unordered_set<entity_t> keepEntities;
        if (_active_entities.find(stateEntity) != _active_entities.end()) {
            keepEntities.insert(stateEntity);
        }

        // Your logic for clearing the game state
        for (auto e : _active_entities) {
            // Skip if this is the state entity
            if (e == stateEntity) {
                continue;
            }

            _free_ids.push_back(e);
            for (auto& [type, eraser] : _eraser_functions) {
                eraser(e);
            }
        }

        // Now, _active_entities will only contain the entities we want to keep
        _active_entities = std::move(keepEntities);

        if (stateEntity != 0) {
            _next_id = 0;
        }else{
            //check in all the free ids and take the biggest one
            for (auto e : _free_ids) {
                if (e > _next_id) {
                    _next_id = e;
                }
            }
        }


    }
    std::unordered_map<entity_t, std::function<void(Registry&, entity_t)>> _entity_constructors;
    std::unordered_set<entity_t> _inactive_entities;
    std::unordered_set<entity_t> _active_entities;
    EventBus eventBus;
    std::unordered_map<int, entity_t> clientToEntityMap;
    //add a ref to keep the entity that hold the State
    entity_t stateEntity;
    bool stateChanged = false;
    bool postUpdateBreaker = false;
private:
    std::unordered_map<std::type_index, std::any> _components_arrays;
    std::unordered_map<std::type_index, std::function<void(entity_t)>> _eraser_functions;
    using SystemFunction = std::function<void(entity_t)>;
    std::vector<std::shared_ptr<ISystemBase>> _system_instances;
    std::vector<SystemFunction> _systems;
    std::vector<entity_t> _free_ids;
    entity_t _next_id = 0;
    std::vector<entity_t> _entities_to_delete;
    std::vector<entity_t> _entities_to_create;  // New member
    std::vector<std::function<void(entity_t, Registry&)>> component_adders;  // Updated type
    std::vector<entity_t> _newly_created_entities;  // New member variable
    std::string path_to_music = "asset/Beatmaps";
    void run_systems_range(std::vector<SystemFunction>::iterator start, std::vector<SystemFunction>::iterator end) {
        std::for_each(start, end, [this](SystemFunction& system) {
            for (entity_t e : _active_entities) {
                system(e);
            }
        });
    }
    std::unordered_map<std::string, std::shared_ptr<ISystemBase>> _system_instance_ids;

};

//tuto source : https://austinmorlan.com/posts/entity_component_system/

#endif //REGISTRY_HPP_