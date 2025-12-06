#include "Constants.hpp"
#include "Logger.hpp"
#include <SFML/Graphics.hpp>

// Define static member
sf::Image Resource::icon;
sf::Music Resource::music;
sf::Texture Resource::titleTexture;
sf::Texture Resource::titleBackgroundTexture;
sf::Font Resource::buttonFont;
sf::Texture Resource::playerTexture;
sf::Texture Resource::guardMonsterTexture;

void Resource::init() {
    // Load icon
    if(!icon.loadFromFile(ICON_FILE)) {
        Logger::log("Failed to load icon file: " + ICON_FILE);
    }

    // Load music
    if(!music.openFromFile(BGM_FILE)) {
        Logger::log("Failed to load BGM file: " + BGM_FILE);
    } else {
        music.play();
        music.setVolume(BGM_VOLUME);
        music.setLooping(true);
    }

    // Load textures
    if(!titleTexture.loadFromFile(TITLE_IMAGE_FILE)) {
        Logger::log("Failed to load title texture: " + TITLE_IMAGE_FILE);
    }

    if(!titleBackgroundTexture.loadFromFile(BACKGROUND_IMAGE_FILE)) {
        Logger::log("Failed to load title background texture: " + BACKGROUND_IMAGE_FILE);
    }
    
    if(!buttonFont.openFromFile(BUTTON_FONT_FILE)) {
        Logger::log("Failed to load button font file: " + BUTTON_FONT_FILE);
    }

    if(!playerTexture.loadFromFile(PLAYER_TEXTURE_FILE)) {
        Logger::log("Failed to load player texture: " + PLAYER_TEXTURE_FILE);
    }

    if(!guardMonsterTexture.loadFromFile(GUARD_MONSTER_TEXTURE_FILE)) {
        Logger::log("Failed to load guard monster texture: " + GUARD_MONSTER_TEXTURE_FILE);
    }
}

const sf::Image& Resource::getIcon() {
    return icon;
}

sf::Music& Resource::getMusic() {
    return music;
}

const sf::Texture& Resource::getTitleTexture() {
    return titleTexture;
}

const sf::Texture& Resource::getTitleBackgroundTexture() {
    return titleBackgroundTexture;
}

const sf::Font& Resource::getButtonFont() {
    return buttonFont;
}

const sf::Texture& Resource::getPlayerTexture() {
    return playerTexture;
}

const sf::Texture& Resource::getGuardMonsterTexture() {
    return guardMonsterTexture;
}