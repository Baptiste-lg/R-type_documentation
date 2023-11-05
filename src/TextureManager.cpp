/*
** EPITECH PROJECT, 2024
** Rtype
** File description:
** TextureManager.cpp
*/

/**
** TextureManager.cpp
**
** This file is part of the Rtype project, created in 2024.
** It implements the functions responsible for loading and managing textures and fonts used in the game.
** The primary function defined is loadAllTexture which is suggested to be renamed to loadAllAssets to better reflect its purpose.
** Additional functions include loadMenuAsset, loadSkins, and preloadGameTexture which handle loading of specific assets for different game scenes.
**
** Functions:
**
** - void loadAllTexture(AssetManager& assetManager):
**   Clears existing textures and loads all necessary textures and a font for the game scene.
**
** - void loadMenuAsset(AssetManager& assetManager, Database& database):
**   Clears existing textures and loads textures and a font for the menu scene. It also loads beatmap covers from a database.
**
** - void loadSkins(AssetManager& assetManager):
**   Loads player skins from the Skins directory into the asset manager.
**
** - void preloadGameTexture(AssetManager& assetManager):
**   Preloads textures for the game scene, similar to loadAllTexture but without clearing existing textures.
**
** Dependencies:
**
** - "TextureManager.hpp" for the declaration of functions.
** - <filesystem> for directory iteration in loadSkins.
** - "AssetManager.hpp" for managing game assets.
** - "Database.hpp" for accessing beatmap information.
**
** Usage:
**
** - Each of these functions is designed to be called at different points in the game lifecycle to ensure that the necessary assets are loaded and ready for use.
** - They leverage the AssetManager class to load and manage textures and fonts.
** - loadSkins specifically iterates through files in a directory to load player skins, ensuring that all available skins are loaded.
**
*/

#include "TextureManager.hpp"

void loadAllTexture(AssetManager& assetManager//TODO rename to loadAllAssets for game
)
{
    //clear before loading
    assetManager.UnloadAllTextures();
    loadSkins(assetManager);
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
    assetManager.LoadTexture("button", "asset/Button11.png");
    assetManager.LoadTexture("spaceship_1", "asset/ennemy_ship/Ship2/Ship2.png");
    assetManager.LoadTexture("spaceship_2", "asset/ennemy_ship/Ship3/Ship3.png");
    assetManager.LoadTexture("spaceship_3", "asset/ennemy_ship/Ship4/Ship4.png");
    //fonts
    assetManager.LoadFont("font", "asset/Fonts/sansation.ttf");
}


void loadMenuAsset(AssetManager& assetManager, Database& database)
{
    //clear before loading
    assetManager.UnloadAllTextures();
    for (int i = 1; i <= 6; ++i) {
        std::string texturePath = "asset/UI/Btn0" + std::to_string(i) + ".png";
        assetManager.LoadTexture("Btn0" + std::to_string(i), texturePath);
    }

    assetManager.LoadTexture( "background","asset/UI/Background.jpg");
    assetManager.LoadTexture("MainPanel01", "asset/UI/MainPanel01.png" );
    assetManager.LoadTexture("ArrowsRight","asset/UI/ArrowsRight.png");
    assetManager.LoadTexture("ArrowsLeft","asset/UI/ArrowsLeft.png" );
    for (int i = 0; i < database.getNbBeatmaps(); ++i) {
        BeatmapConfig beatmap = database.getBeatmap(i);
        if (beatmap.getFolderPath() == "") {
            continue;
        }
        std::string coverPath = "asset/Beatmaps/" + beatmap.getFolderPath() + "/Cover.jpg";
        assetManager.LoadTexture(beatmap.getFolderPath(), coverPath);
    }
    //fonts
    assetManager.LoadFont("font", "asset/Fonts/sansation.ttf");


    //load the lobby texture
    assetManager.LoadTexture("LobbyBackground", "asset/UI/lobby_background.jpg");
    assetManager.LoadTexture("LobbyButtonLeft", "asset/UI/lobby_button_left.png");
    assetManager.LoadTexture("LobbyButtonRight", "asset/UI/lobby_button_right.png");
    assetManager.LoadTexture("SkinButton", "asset/UI/Btn02.png");
    assetManager.LoadTexture("LobbyHolderBig", "asset/UI/lobby_holder_big.png");
    assetManager.LoadTexture("LobbyHolderSmall", "asset/UI/lobby_holder_small.png");
    assetManager.LoadTexture("LobbyGridHolder", "asset/UI/lobby_grid_holder.png");
    assetManager.LoadTexture("lobbyPlayerIcon", "asset/UI/lobby_player_icon.png");
    assetManager.LoadTexture("black.png", "asset/black.png");
    assetManager.LoadTexture("empty", "asset/UI/transparent.png");
    assetManager.LoadTexture("e_ship1", "asset/ennemy_ship/Ship2/Ship2.png");

}

void loadSkins(AssetManager& assetManager)
{
    assetManager.LoadTexture("skin_default", "asset/Spaceship/vehicle-1.png");
    std::string skinDirectoryPath = "asset/Skins";

    if (!std::filesystem::exists(skinDirectoryPath)) {
        std::cerr << "Error: Skins directory does not exist: " << skinDirectoryPath << std::endl;
        return;  // Exit the function if the directory does not exist
    }
    for (const auto& entry : std::filesystem::directory_iterator(skinDirectoryPath))
    {
        if (entry.is_regular_file()) { // Make sure it's a file, not a directory
            std::string filePath = entry.path().string();
            std::string fileName = entry.path().filename().string();
            // Remove the file extension, assuming it's '.png'
            std::string skinName = "skin_" + fileName.substr(0, fileName.size() - 4);
            assetManager.LoadTexture(skinName, filePath);
        }
    }
}

void preloadGameTexture(AssetManager& assetManager//TODO rename to loadAllAssets for game
)
{
    //clear before loading
    assetManager.LoadTexture("green_planet", "asset/GreenPlanet.png");
    assetManager.LoadTexture("bddackground", "asset/layers/parallax-space-backgound.png");
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
    assetManager.LoadTexture("button", "asset/Button11.png");
    assetManager.LoadTexture("spaceship_1", "asset/ennemy_ship/Ship2/Ship2.png");
    assetManager.LoadTexture("spaceship_2", "asset/ennemy_ship/Ship3/Ship3.png");
    assetManager.LoadTexture("spaceship_3", "asset/ennemy_ship/Ship4/Ship4.png");
    //fonts
    assetManager.LoadFont("font", "asset/Fonts/sansation.ttf");
}
