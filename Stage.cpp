#include "Constants.hpp"
#include "Utils.hpp"
#include "Logger.hpp"
#include "Stage.hpp"
#include <iostream>
#include <vector>

Entity::Entity(int x, int y, const sf::Texture& texture) : posX(x), posY(y), sprite(texture) {}

// Attempt to move the entity in the specified direction
// Returns true if the move was successful, false otherwise

void Entity::draw(sf::RenderWindow& window) {
    // To be determined
    sprite.setPosition({posX * 100.f, posY * 100.f});
    // Logger::log("Drawing entity at position: (" + std::to_string(posX * 100.f) + ", " 
    //     + std::to_string(posY * 100.f) + ")");
    window.draw(sprite);
}

Player::Player(int x, int y, const sf::Texture& texture) : Entity(x, y, texture) {
    Logger::log("Player created at tile: (" + std::to_string(x) + ", " + std::to_string(y) + ")");
}

void Player::interact(Entity& entity) {
    // To be determined
}



// ========== Stage Class =============
Stage::Stage(int stageNum, int column, int row) : stageNum(stageNum), row(row), column(column), player(1, 1, Resource::getPlayerTexture()) {
    // Initialize tileMap with empty tiles
    tileMap.resize(row, std::vector<char>(column, 'O')); // 'O' represents open space
    createTiles(shapes, column, row, 100, 100);
    Logger::log("Stage " + std::to_string(stageNum) + " created with size (" + std::to_string(column) + ", " + std::to_string(row) + ").");
    
    Logger::log("Player initialized in Stage " + std::to_string(stageNum) + ".");

    print();
}

Stage::~Stage() {
    for(auto shape : shapes) {
        delete shape;
    }
}

int Stage::getRow() const { return row; }
int Stage::getColumn() const { return column; }
Player& Stage::getPlayer() { return player; }

bool Stage::moveEntitySuccessful(Entity& entity, std::string direction) {
    if(direction == "W" && entity.posY > 0) {
        entity.posY -= 1;
        Logger::log("Move upward.");
        return true;
    } else if(direction == "S" && entity.posY < row - 1) {
        entity.posY += 1;
        Logger::log("Move downward.");
        return true;
    } else if(direction == "A" && entity.posX > 0) {
        entity.posX -= 1;
        Logger::log("Move right.");
        return true;
    } else if(direction == "D" && entity.posX < column - 1) {
        entity.posX += 1;
        Logger::log("Move left.");
        return true;
    }
    Logger::log("Move failed.");
    return false;
}

void Stage::draw(sf::RenderWindow& window) {
    // Draw all shapes
    for(const auto& shape : shapes) {
        window.draw(*shape);
    }

    // Draw player
    player.draw(window);
}

void Stage::print() const {
    Logger::log_debug("Printing tile map for Stage " + std::to_string(stageNum) + ":");
    Logger::log_debug("Size (" + std::to_string(column) + ", " + std::to_string(row) + "):");
    Logger::log_debug("---------------------");
    for(const auto& row : tileMap) {
        std::string line;
        for(const auto& tile : row) {
            line += tile;
        }
        Logger::log_debug(line);
    }
    Logger::log_debug("---------------------");
}