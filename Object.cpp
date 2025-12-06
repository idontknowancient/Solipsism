#include "Logger.hpp"
#include "Utils.hpp"
#include "Object.hpp"

Object::Object(const sf::Texture& texture, sf::Vector2i posTile, sf::Vector2f posWindow, int tile_size) 
    : sprite(texture), posTile(posTile), posWindow(posWindow) {
    resizeTileTexture(sprite, tile_size);
    sprite.setPosition(posWindow);
}

sf::Sprite& Object::getSprite() {
    return sprite;
}

void Object::draw(sf::RenderWindow& window, int tile_size) {
    window.draw(sprite);
}


bool Object::isValidMove(std::vector<std::vector<char>>& tileMap, sf::Vector2i newPosTile) {
    int column = tileMap[0].size();
    int row = tileMap.size();

    // Check bounds
    if(newPosTile.x < 0 || newPosTile.x >= column || newPosTile.y < 0 || newPosTile.y >= row) {
        Logger::log_debug("Move out of bounds to (" + std::to_string(newPosTile.x) + ", " + std::to_string(newPosTile.y) + ").");
        return false;
    }

    // Check tile type
    char tileType = tileMap[newPosTile.y][newPosTile.x];
    if(tileType == 'X') { // Wall
        Logger::log_debug("Move blocked by wall at (" + std::to_string(newPosTile.x) + ", " + std::to_string(newPosTile.y) + ").");
        return false;
    }

    return true; // Valid move
}



// ========== Player Class =============
Player::Player(sf::Vector2i posTile, sf::Vector2f posWindow, int tile_size) 
    : Object(Resource::getPlayerTexture(), posTile, posWindow, tile_size) {
    Logger::log("Player created at tile (" 
        + std::to_string(posTile.x) + ", " + std::to_string(posTile.y) + ").");
}

bool Player::isValidMove(std::vector<std::vector<char>>& tileMap, const Action& action) {
    sf::Vector2i newPos = posTile;
    if(action == Action::MoveUp) {
        newPos.y -= 1;
    } else if(action == Action::MoveDown) {
        newPos.y += 1;
    } else if(action == Action::MoveLeft) {
        newPos.x -= 1;
    } else if(action == Action::MoveRight) {
        newPos.x += 1;
    }

    return Object::isValidMove(tileMap, newPos);
}

void Player::update(std::vector<std::vector<char>>& tileMap, int tile_size, const Action& action) {
    if(!isValidMove(tileMap, action)) return;

    // Update tile and window position
    if(action == Action::MoveUp) {
        posTile.y -= 1;
        posWindow.y -= tile_size;
    } else if(action == Action::MoveDown) {
        posTile.y += 1;
        posWindow.y += tile_size;
    } else if(action == Action::MoveLeft) {
        posTile.x -= 1;
        posWindow.x -= tile_size;
    } else if(action == Action::MoveRight) {
        posTile.x += 1;
        posWindow.x += tile_size;
    }
    getSprite().setPosition(posWindow);

    Logger::log_debug("Player moved to (" 
        + std::to_string(posTile.x) + ", " 
        + std::to_string(posTile.y) + ").");
}



// ========== Monster Class =============
bool Monster::isValidMove(std::vector<std::vector<char>>& tileMap, char actionChar) {
    sf::Vector2i newPos = posTile;
    if(actionChar == 'U') {
        newPos.y -= 1;
    } else if(actionChar == 'D') {
        newPos.y += 1;
    } else if(actionChar == 'L') {
        newPos.x -= 1;
    } else if(actionChar == 'R') {
        newPos.x += 1;
    }

    return Object::isValidMove(tileMap, newPos);
}

TraceMonster::TraceMonster(sf::Vector2i posTile, sf::Vector2f posWindow, int tile_size) : 
    Monster(Resource::getTraceMonsterTexture(), posTile, posWindow, tile_size) {
    Logger::log("TraceMonster created at tile (" 
        + std::to_string(posTile.x) + ", " + std::to_string(posTile.y) + ").");
}

void TraceMonster::update(std::vector<std::vector<char>>& tileMap, int tile_size) {
    // Simple trace logic: move down if possible, else up
    char actionChar = 'D';
    if(!isValidMove(tileMap, actionChar)) {
        actionChar = 'U';
        if(!isValidMove(tileMap, actionChar)) {
            return; // No valid move
        }
    }

    // Update tile and window position
    if(actionChar == 'U') {
        posTile.y -= 1;
        posWindow.y -= tile_size;
        tileMap[posTile.y + 1][posTile.x] = '-'; // Clear previous position
        tileMap[posTile.y][posTile.x] = 'm'; // Move to new position
    } else if(actionChar == 'D') {
        posTile.y += 1;
        posWindow.y += tile_size;
        tileMap[posTile.y - 1][posTile.x] = '-'; // Clear previous position
        tileMap[posTile.y][posTile.x] = 'm'; // Move to new position
    }
    getSprite().setPosition(posWindow);

    Logger::log_debug("TraceMonster moved to (" 
        + std::to_string(posTile.x) + ", " 
        + std::to_string(posTile.y) + ").");
}

GuardMonster::GuardMonster(sf::Vector2i posTile, sf::Vector2f posWindow, int tile_size, const std::string& pattern) : 
    Monster(Resource::getGuardMonsterTexture(), posTile, posWindow, tile_size), behaviorPattern(pattern) {
    Logger::log("GuardMonster created at tile (" 
        + std::to_string(posTile.x) + ", " + std::to_string(posTile.y) + ") "
        + "with behavior pattern: " + behaviorPattern);
}

void GuardMonster::update(std::vector<std::vector<char>>& tileMap, int tile_size) {
    if(behaviorPattern.empty()) return;

    char actionChar = behaviorPattern[0];
    if(!isValidMove(tileMap, actionChar)) return;

    cyclePattern(behaviorPattern);

    // Update tile and window position
    if(actionChar == 'U') {
        posTile.y -= 1;
        posWindow.y -= tile_size;
        tileMap[posTile.y + 1][posTile.x] = '-'; // Clear previous position
        tileMap[posTile.y][posTile.x] = 'm'; // Move to new position
    } else if(actionChar == 'D') {
        posTile.y += 1;
        posWindow.y += tile_size;
        tileMap[posTile.y - 1][posTile.x] = '-'; // Clear previous position
        tileMap[posTile.y][posTile.x] = 'm'; // Move to new position
    } else if(actionChar == 'L') {
        posTile.x -= 1;
        posWindow.x -= tile_size;
        tileMap[posTile.y][posTile.x + 1] = '-'; // Clear previous position
        tileMap[posTile.y][posTile.x] = 'm'; // Move to new position
    } else if(actionChar == 'R') {
        posTile.x += 1;
        posWindow.x += tile_size;
        tileMap[posTile.y][posTile.x - 1] = '-'; // Clear previous position
        tileMap[posTile.y][posTile.x] = 'm'; // Move to new position
    }
    getSprite().setPosition(posWindow);

    Logger::log_debug("GuardMonster moved to (" 
        + std::to_string(posTile.x) + ", " 
        + std::to_string(posTile.y) + ").");
}

std::string& GuardMonster::getBehaviorPattern() {
    return behaviorPattern;
}