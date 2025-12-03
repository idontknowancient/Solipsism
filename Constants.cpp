#include "Constants.hpp"
#include "Logger.hpp"
#include <SFML/Graphics.hpp>

// Define static member
sf::Texture Resource::playerTexture;

void Resource::init() {
    if(!playerTexture.loadFromFile(PLAYER_TEXTURE_FILE)) {
        Logger::log("Failed to load player texture: " + PLAYER_TEXTURE_FILE);
    }
}

const sf::Texture& Resource::getPlayerTexture() {
    return playerTexture;
}