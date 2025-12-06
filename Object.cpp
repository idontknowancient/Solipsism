#include "Logger.hpp"
#include "Utils.hpp"
#include "Object.hpp"

Object::Object(const sf::Texture& texture) : sprite(texture), posTile(0, 0), posWindow(0.f, 0.f) {}

sf::Sprite& Object::getSprite() {
    return sprite;
}

void Object::draw(sf::RenderWindow& window, int tile_size) {
    window.draw(sprite);
}



// ========== Player Class =============
Player::Player(sf::Vector2i posTile, sf::Vector2f posWindow, int tile_size) : Object(Resource::getPlayerTexture()) {
    this->posTile = posTile;
    this->posWindow = posWindow;
    Logger::log("Player created at tile (" 
        + std::to_string(posTile.x) + ", " + std::to_string(posTile.y) + ").");
    sprite.setPosition(posWindow);

    resizeTileTexture(this->sprite, tile_size);
}

void Player::update(std::vector<std::vector<char>>& tileMap, int tile_size) {
    // To be determined
}



// ========== Monster Class =============
GuardMonster::GuardMonster(sf::Vector2i posTile, sf::Vector2f posWindow, int tile_size, const std::string& pattern) : 
    Monster(Resource::getGuardMonsterTexture()), behaviorPattern(pattern) {
    this->posTile = posTile;
    this->posWindow = posWindow;
    Logger::log("GuardMonster created at tile (" 
        + std::to_string(posTile.x) + ", " + std::to_string(posTile.y) + ") "
        + "with behavior pattern: " + behaviorPattern);
    sprite.setPosition(posWindow);

    resizeTileTexture(this->sprite, tile_size);
}

void GuardMonster::update(std::vector<std::vector<char>>& tileMap, int tile_size) {
    int column = tileMap[0].size();
    int row = tileMap.size();
    
    // Resolve movement based on behaviorPattern
    char actionChar = behaviorPattern[0];
    if(actionChar == 'U') {
        if(posTile.y > 0 && tileMap[posTile.y - 1][posTile.x] == '-') {
            tileMap[posTile.y][posTile.x] = '-'; // Clear current position
            tileMap[posTile.y - 1][posTile.x] = 'm'; // Move to new position
            posTile.y -= 1;
            posWindow.y -= tile_size;
        }
        cyclePattern(behaviorPattern);
    } else if(actionChar == 'D') {
        if(posTile.y < row - 1 && tileMap[posTile.y + 1][posTile.x] == '-') {
            tileMap[posTile.y][posTile.x] = '-'; // Clear current position
            tileMap[posTile.y + 1][posTile.x] = 'm'; // Move to new position
            posTile.y += 1;
            posWindow.y += tile_size;
        }
        cyclePattern(behaviorPattern);
    } else if(actionChar == 'L') {
        if(posTile.x > 0 && tileMap[posTile.y][posTile.x - 1] == '-') {
            tileMap[posTile.y][posTile.x] = '-'; // Clear current position
            tileMap[posTile.y][posTile.x - 1] = 'm'; // Move to new position
            posTile.x -= 1;
            posWindow.x -= tile_size;
        }
        cyclePattern(behaviorPattern);
    } else if(actionChar == 'R') {
        if(posTile.x < column - 1 && tileMap[posTile.y][posTile.x + 1] == '-') {
            tileMap[posTile.y][posTile.x] = '-'; // Clear current position
            tileMap[posTile.y][posTile.x + 1] = 'm'; //
            posTile.x += 1;
            posWindow.x += tile_size;
        }
        cyclePattern(behaviorPattern);
    }
    sprite.setPosition(posWindow);
}

std::string& GuardMonster::getBehaviorPattern() {
    return behaviorPattern;
}