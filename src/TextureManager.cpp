
#include "TextureManager.hpp"


void loadAllTexture(AssetManager& assetManager)
{
    assetManager.LoadTexture("green_planet", "asset/GreenPlanet.png");
    assetManager.LoadTexture("background", "asset/layers/parallax-space-backgound.png");
    assetManager.LoadTexture("big_planet", "asset/layers/parallax-space-big-planet.png");
    assetManager.LoadTexture("far_planets", "asset/layers/parallax-space-far-planets.png");
    assetManager.LoadTexture("ring_planet", "asset/layers/parallax-space-ring-planet.png");
    assetManager.LoadTexture("stars", "asset/layers/parallax-space-stars.png");
    assetManager.LoadTexture("laser", "asset/Spaceship/laser.png");
    assetManager.LoadTexture("laserAnimated", "asset/Spaceship/shootingSheet.png");
    assetManager.LoadTexture("p_spaceship", "asset/Spaceship/vehicle-1.png");
    assetManager.LoadTexture("p_thruster", "asset/Spaceship/thrust/thrust.png");
    assetManager.LoadTexture("e_ship1", "asset/ennemy_ship/Ship2/Ship2.png");
    assetManager.LoadTexture("e_ship2", "asset/ennemy_ship/Ship3/Ship3.png");
    assetManager.LoadTexture("e_ship3", "asset/ennemy_ship/Ship4/Ship4.png");

}