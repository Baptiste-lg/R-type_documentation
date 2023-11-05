/*
** EPITECH PROJECT, 2024
** Rtype
** File description:
** textureManager
*/

/**
 * TextureManager.hpp handles loading and managing game assets.
 *
 * Loads textures, sounds, fonts to be used by other game systems.
 * Textures and sounds are reference counted and unloaded when no longer used.
 *
 * Usage:
 * - GetTexture(), GetSound(), GetFont() to retrieve assets.
 * - LoadTexture(), LoadSound(), LoadFont() to load from files.
 * - UnloadTexture(), UnloadSound() to decrement ref count.
 * - PreloadTextures(), PreloadSounds() to load ahead of time.
 *
 * Methods:
 * - Load/Get methods to add and retrieve assets.
 * - Unload methods decrement ref count and remove.
 * - Preload methods load assets ahead of first use.
 *
 * Members:
 * - textures: Map of texture name to SFML texture.
 * - sounds: Map of sound name to SFML sound buffer.
 * - fonts: Map of font name to SFML font.
 * - refCount maps: Reference count for each asset.
 *
* Singleton pattern:
* - Private constructor, public static Instance() method.
* - Ensures only one TextureManager instance.
*/


#ifndef ASSETMANAGER_HPP_
#define ASSETMANAGER_HPP_

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <filesystem>
#include <unordered_map>
#include <string>
#include <iostream>
#include <vector>
#include "Database/Database.hpp"

class AssetManager {
public:
    static AssetManager& Instance() {
        static AssetManager instance;
        return instance;
    }

    sf::Texture& GetTexture(const std::string& name) {
        if (textures.find(name) != textures.end()) {
            textureRefCount[name]++;
            return textures[name];
        }
        // Handle error
        std::cerr << "Texture not found: " << name << std::endl;
        return textures["missing_texture"];
    }

    void LoadTexture(const std::string& name, const std::string& filename) {
        sf::Texture tex = sf::Texture();
        if (tex.loadFromFile(filename)) {
            textures[name] = tex;
            textureRefCount[name] = 1;  // Initialize reference count
        } else {
            std::cerr << "Failed to load texture from: " << filename << std::endl;
        }
    }

    void UnloadTexture(const std::string& name) {
        if (--textureRefCount[name] == 0) {
            textures.erase(name);
        }
    }
    void UnloadAllTextures() {
        textures.clear();
        textureRefCount.clear();
    }

    sf::SoundBuffer& GetSound(const std::string& name) {
        if (sounds.find(name) != sounds.end()) {
            soundRefCount[name]++;
            return sounds[name];
        }
        // Handle error
        std::cerr << "Sound not found: " << name << std::endl;
        return sounds["missing_sound"];
    }

    void LoadSound(const std::string& name, const std::string& filename) {
        sf::SoundBuffer sound;
        if (sound.loadFromFile(filename)) {
            sounds[name] = sound;
            soundRefCount[name] = 1;  // Initialize reference count
        } else {
            std::cerr << "Failed to load sound from: " << filename << std::endl;
        }
    }



    void UnloadSound(const std::string& name) {
        if (--soundRefCount[name] == 0) {
            sounds.erase(name);
        }
    }

    void UnloadAllSounds() {
        sounds.clear();
        soundRefCount.clear();
    }

    void PreloadTextures(const std::vector<std::string>& textureNames) {
        for (const auto& textureName : textureNames) {
            // Logic to preload textures
        }
    }

    void PreloadSounds(const std::vector<std::string>& soundNames) {
        // Logic to preload sounds
    }

    sf::Font& GetFont(const std::string& name) {
        if (fonts.find(name) != fonts.end()) {
            fontRefCount[name]++;
            return fonts[name];
        }
        // Handle error
        std::cerr << "Font not found: " << name << std::endl;
        return fonts["missing_font"];
    }

    void LoadFont(const std::string& name, const std::string& filename) {
        sf::Font font;
        if (font.loadFromFile(filename)) {
            fonts[name] = font;
            fontRefCount[name] = 1;  // Initialize reference count
        } else {
            std::cerr << "Failed to load font from: " << filename << std::endl;
        }
    }

    std::unordered_map<std::string, sf::Texture>& getTextures() {
        return textures;
    }

private:
    AssetManager() = default;
    std::unordered_map<std::string, sf::Texture> textures;
    std::unordered_map<std::string, int> textureRefCount;

    std::unordered_map<std::string, sf::SoundBuffer> sounds;
    std::unordered_map<std::string, int> soundRefCount;

    std::unordered_map<std::string, sf::Font> fonts;
    std::unordered_map<std::string, int> fontRefCount;
};

void loadAllTexture(AssetManager& assetManager);
void loadMenuAsset(AssetManager& assetManager, Database& database);
void loadSkins(AssetManager& assetManager);
void preloadGameTexture(AssetManager& assetManager);
#endif // ASSETMANAGER_HPP_
