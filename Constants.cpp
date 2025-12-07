#include "Constants.hpp"
#include "Logger.hpp"
#include <SFML/Graphics.hpp>

// Define static member
sf::Image Resource::icon;
sf::Music Resource::music;
sf::Texture Resource::titleTexture;
sf::Texture Resource::backgroundTitleTexture;
sf::Texture Resource::backgroundStageTexture;
sf::Texture Resource::stageClearTexture;
sf::Font Resource::buttonFont;
sf::Texture Resource::playerTexture;
sf::Texture Resource::openSpaceTexture1;
sf::Texture Resource::openSpaceTexture2;
sf::Texture Resource::openSpaceTexture3;
sf::Texture Resource::openSpaceTexture4;
sf::Texture Resource::wallTexture;
sf::Texture Resource::goalTexture;
sf::Texture Resource::guardMonsterTexture;
sf::Texture Resource::traceMonsterTexture;
sf::Texture Resource::dispenserTexture;
sf::Texture Resource::arrowTexture;
sf::Texture Resource::trapTexture;

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

    if(!backgroundTitleTexture.loadFromFile(BACKGROUND_TITLE_FILE)) {
        Logger::log("Failed to load title background texture: " + BACKGROUND_TITLE_FILE);
    }

    if(!backgroundStageTexture.loadFromFile(BACKGROUND_STAGE_FILE)) {
        Logger::log("Failed to load stage background texture: " + BACKGROUND_STAGE_FILE);
    }

    if(!stageClearTexture.loadFromFile(STAGE_CLEAR_FILE)) {
        Logger::log("Failed to load stage clear texture: " + STAGE_CLEAR_FILE);
    }
    
    if(!buttonFont.openFromFile(BUTTON_FONT_FILE)) {
        Logger::log("Failed to load button font file: " + BUTTON_FONT_FILE);
    }

    if(!playerTexture.loadFromFile(PLAYER_TEXTURE_FILE)) {
        Logger::log("Failed to load player texture: " + PLAYER_TEXTURE_FILE);
    }

    if(!openSpaceTexture1.loadFromFile(OPEN_SPACE_1_TEXTURE_FILE)) {
        Logger::log("Failed to load open space texture 1: " + OPEN_SPACE_1_TEXTURE_FILE);
    }

    if(!openSpaceTexture2.loadFromFile(OPEN_SPACE_2_TEXTURE_FILE)) {
        Logger::log("Failed to load open space texture 2: " + OPEN_SPACE_2_TEXTURE_FILE);
    }

    if(!openSpaceTexture3.loadFromFile(OPEN_SPACE_3_TEXTURE_FILE)) {
        Logger::log("Failed to load open space texture 3: " + OPEN_SPACE_3_TEXTURE_FILE);
    }

    if(!openSpaceTexture4.loadFromFile(OPEN_SPACE_4_TEXTURE_FILE)) {
        Logger::log("Failed to load open space texture 4: " + OPEN_SPACE_4_TEXTURE_FILE);
    }

    if(!wallTexture.loadFromFile(WALL_TEXTURE_FILE)) {
        Logger::log("Failed to load wall texture: " + WALL_TEXTURE_FILE);
    }

    if(!goalTexture.loadFromFile(GOAL_TEXTURE_FILE)) {
        Logger::log("Failed to load goal texture: " + GOAL_TEXTURE_FILE);
    }

    if(!guardMonsterTexture.loadFromFile(GUARD_MONSTER_TEXTURE_FILE)) {
        Logger::log("Failed to load guard monster texture: " + GUARD_MONSTER_TEXTURE_FILE);
    }

    if(!traceMonsterTexture.loadFromFile(TRACE_MONSTER_TEXTURE_FILE)) {
        Logger::log("Failed to load trace monster texture: " + TRACE_MONSTER_TEXTURE_FILE);
    }

    if(!dispenserTexture.loadFromFile(DISPENSER_TEXTURE_FILE)) {
        Logger::log("Failed to load dispenser texture: " + DISPENSER_TEXTURE_FILE);
    }

    if(!arrowTexture.loadFromFile(ARROW_TEXTURE_FILE)) {
        Logger::log("Failed to load arrow texture: " + ARROW_TEXTURE_FILE);
    }

    if(!trapTexture.loadFromFile(TRAP_TEXTURE_FILE)) {
        Logger::log("Failed to load trap texture: " + TRAP_TEXTURE_FILE);
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

const sf::Texture& Resource::getBackgroundTitleTexture() {
    return backgroundTitleTexture;
}

const sf::Texture& Resource::getBackgroundStageTexture() {
    return backgroundStageTexture;
}

const sf::Texture& Resource::getStageClearTexture() {
    return stageClearTexture;
}

const sf::Font& Resource::getButtonFont() {
    return buttonFont;
}

const sf::Texture& Resource::getPlayerTexture() {
    return playerTexture;
}

const sf::Texture& Resource::getOpenSpaceTexture(int variant) {
    switch(variant) {
        case 1: return openSpaceTexture1;
        case 2: return openSpaceTexture2;
        case 3: return openSpaceTexture3;
        case 4: return openSpaceTexture4;
        default: return openSpaceTexture1; // Default to variant 1 if invalid
    }
}

const sf::Texture& Resource::getWallTexture() {
    return wallTexture;
}

const sf::Texture& Resource::getGoalTexture() {
    return goalTexture;
}

const sf::Texture& Resource::getGuardMonsterTexture() {
    return guardMonsterTexture;
}

const sf::Texture& Resource::getTraceMonsterTexture() {
    return traceMonsterTexture;
}

const sf::Texture& Resource::getDispenserTexture() {
    return dispenserTexture;
}

const sf::Texture& Resource::getArrowTexture() {
    return arrowTexture;
}

const sf::Texture& Resource::getTrapTexture() {
    return trapTexture;
}