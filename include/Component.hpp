/*
** EPITECH PROJECT, 2024
** Rtype
** File description:
** COMPONENT
*/

/**
** Component
**
** This file defines component structs used in the ECS (Entity Component System).
**
** Component Structs:
**
** - Position
**   Stores x and y coordinate floats.
**
** - Collision
**   Stores entity type, collider type and colliding entity info.
**
** - TextDrawable
**   Stores text, font name, font size, and color.
**
** - Velocity
**   Stores x and y velocity floats.
**
** - Acceleration
**   Stores x and y acceleration floats.
**
** - Follower
**   Stores target entity and x/y offsets.
**
** - Drawable
**   Stores texture name, source rect, and scale.
**
** - Animation
**   Stores animation frames and timing data.
**
** - isClickable
**   Stores click handling data.
**
** - GameConfig
**   Stores game configuration data.
**
** - Controllable
**   Stores local control and client ID data.
**
** - Laser
**
** - Depth
**   Stores z depth float.
**
** - Behavior
**   Stores behavior ID and origin coords.
**
** - Enemy
**   Stores enemy health points.
**
** - FFT
**
** - Spawner
**   Stores spawning configuration and timing.
**
** - Health
**   Stores health points.
**
** - Network
**   Stores network connection data.
**
** - State
**   Stores current game state enum.
**
** - Client
**   Stores client connection data.
**
** - UserInput
**   Stores user input character.
**
** - UserInputBuffer
**   Stores user input character buffer.
**
** - DeathEffect
**   Stores death effect name and function.
**
** - idSync
**   Stores synced ID integer.
**
** - IsChild
**   Stores parent entity ID.
**
*/

#ifndef COMPONENT_HPP_
#define COMPONENT_HPP_
#include <SFML/Graphics.hpp>
#include <libconfig.h++>
#include <any>
#include "Registry.hpp"
#include "TextureManager.hpp"
#include <complex>
#include <valarray>
#include <cmath>
#include <vector>
#include <string>
#include <memory>
#include <algorithm>
#include <numeric>
#include <functional>
#include <utility>
#include <array>
#include <map>
#include <set>
#include <iostream>

using Complex = std::complex<double>;
using ComplAr = std::valarray<Complex>;
const float PI = static_cast<float>(3.14159265358979323846);
class Registry;
using ComponentAdder = std::function<void(entity_t, Registry&, const libconfig::Setting&)>;
using ComponentSpawner = std::function<void(entity_t, Registry&)>;
struct MyRect {
    int left, top, width, height;
};

struct MyVector2 {
    float x, y;
};

enum ColliderType {
    STOP,
    BOUNCING,
    DESTROY
};

enum EntityType {
    ENEMY,     // Represents an enemy entity.
    FRIENDLY,  // Represents a friendly or neutral entity.
    FRIENDLY_HOST,
    FRIENDLY_BULLET,
    FRIENDLY_PROJECTILE_HOST,
    OBSTACLE   // Represents an environmental obstacle.
};

enum DeathApplyType {
    TO_HOST,
    TO_CLIENT,
    TO_ALL,
    TO_PARENT
};
extern std::unordered_map<std::string, ColliderType> colliderTypeMap;

extern std::unordered_map<std::string, EntityType> entityTypeMap;
extern std::unordered_map<std::string, DeathApplyType> deathApplyTypeMap;
//component structs
struct Position
{
    float x = 0.0f;
    float y = 0.0f;
    float prevX = 0.0f;
    float prevY = 0.0f;
    friend std::ostream& operator<<(std::ostream& os, const Position& pos) {
        os << "Position(x: " << pos.x << ", y: " << pos.y << ")";
        return os;
    }
    friend bool operator==(const Position& lhs, const Position& rhs) {
        return lhs.x == rhs.x && lhs.y == rhs.y;
    }

};

struct Collision {
    EntityType entityType = EntityType::OBSTACLE;
    ColliderType colliderType = ColliderType::DESTROY;
    // std::string collidingEntity;
};
struct TextDrawable {
    std::string text;
    std::string fontName;  // Reference to a font asset
    unsigned int fontSize = 30;
    sf::Color color = sf::Color::White;
};


struct Velocity
{
    float x = 0.0f;
    float y = 0.0f;
    friend std::ostream& operator<<(std::ostream& os, const Velocity& vel) {
        os << "Velocity(x: " << vel.x << ", y: " << vel.y << ")";
        return os;
    }
};

struct Acceleration {
    float x = 0.0f;
    float y = 0.0f;
};

struct Follower {
    entity_t target;
    float xOffset = 0.0f;
    float yOffset = 0.0f;
};

struct Drawable {
    std::string textureName;
    MyRect textureRect; // For animation frames
    MyVector2 scale = {1.0f, 1.0f};  // Scale factor
};

struct Animation {
    int currentFrame = 0;  // Current frame of animation
    int numFrames = 1;  // Number of frames in animation
    float frameDuration = 0.0f;  // Duration of each frame in seconds
    float elapsedTime = 0.0f;  // Time elapsed since last frame change
    bool loop = true;  // Whether to loop the animation
};
struct isClickable {
    bool isClicked = false;
    std::function<void()> onClick;
    float elapsedTimeSinceLastClick = 0.0f;
};

struct GameConfig {
    float screenWidth = 0.0f;
    float screenHeight = 0.0f;
    float playerSpeed = 0.0f;
    int playerScore = 0;
    std::string playerSkin = "skin_default";
};

struct Controllable
{
    bool isLocal = false;
    int client_id = 0;
};

struct Laser {

};
struct Depth {
    float z = 0.0f;
};
struct Behavior {
    std::string behaviorID;
    float originX = 0.0f;
    float originY = 0.0f;
};

struct Enemy {
    size_t health = 20;
};

struct FFT {

};


struct Spawner {
    float spawnRate = 0.0f;
    float timeSinceLastSpawn = 0.0f;
    std::string entityToSpawn;
    float xOffset = 0.0f;
    float yOffset = 0.0f;
    float xVelocity = 0.0f;
    float yVelocity = 0.0f;
    std::vector<ComponentSpawner > componentSpawners;
};


struct Health {
    float health = 100.0f;
};

struct Network {
    int socket;
    std::string ip;
    int port;
    int client_id;
    int latency;
    int last_packet_time;
    int last_packet_id;
};

struct State
{
    GameState state = GameState::MENUPRELOAD;
};

struct Client
{
    int id;
    std::string ip;
    entity_t entity_id;
    int port;
    int latency;
    int last_packet_time;
    int last_packet_id;
};

struct UserInput {
    char input;
};

struct UserInputBuffer {
    std::vector<char> buffer;
};

struct DeathEffect {
    std::string Name;
    std::function<void(Registry &)> Effect;
    int applyTo = DeathApplyType::TO_HOST;
};
struct idSync {
    int id;
};

struct IsChild {
    entity_t parent;
};

void register_all_components(Registry& registry);
void registerClientSystem(Registry& registry, sf::RenderWindow& window, AssetManager& textureManager,float &elapsed_time);
void registerServerSystem(Registry& registry, sf::RenderWindow& window, AssetManager& textureManager, float &elapsed_time);
void cleanup_system(Registry& registry);
void addEnemyMovementSystem(Registry& registry);


#endif /* COMPONENT_HPP_ */