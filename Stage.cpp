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
Stage::Stage(int stageId, int column, int row) : stageId(stageId), row(row), column(column), player(0, 0, Resource::getPlayerTexture()) {
    // Initialize tile map with open space '-'
    tileMap.resize(row, std::vector<char>(column, '-'));
    createTiles(shapes, column, row, 100, 100);
    Logger::log("Stage " + std::to_string(stageId) + " created with size (" 
        + std::to_string(column) + ", " + std::to_string(row) + ").");
}

Stage::~Stage() {
    for(auto shape : shapes) {
        delete shape;
    }
}

void Stage::createFromFile(std::vector<Stage>& stages) {
    std::ifstream file(STAGE_FILE);
    if(!file.is_open()) {
        Logger::log("Failed to open " + STAGE_FILE + " for stages creation.");
        return;
    }

    std::string line;
    while(std::getline(file, line)) {
        if(line != "STAGE_START") continue; // Find next stage block

        int stageId = 0;
        int column = 0;
        int row = 0;

        // Parse header lines: STAGE_ID, COLUMN, ROW
        while(std::getline(file, line)) {
            if(line.rfind("STAGE_ID:", 0) == 0) {
                stageId = std::stoi(line.substr(std::string("STAGE_ID:").size()));
                Logger::log("Loading Stage: " + std::to_string(stageId));
            } else if(line.rfind("COLUMN:", 0) == 0) {
                column = std::stoi(line.substr(std::string("COLUMN:").size()));
            } else if(line.rfind("ROW:", 0) == 0) {
                row = std::stoi(line.substr(std::string("ROW:").size()));
            } else if(line == "MAP_START") {
                break; // Proceed to map parsing
            }
        }

        if(stageId <= 0 || column <= 0 || row <= 0) {
            Logger::log("Invalid stage header encountered. Skipping.");
            // Skip to next STAGE_END
            while(std::getline(file, line) && line != "STAGE_END");
            continue;
        }

        Stage stage(stageId, column, row);

        // Parse map lines until MAP_END
        int r = 0;
        while(std::getline(file, line)) {
            if(line == "MAP_END") break;
            if(line.size() >= 2 && line[0] == '#' && line[1] == '#') continue; // skip comment lines starting with '##'
            if(line.empty()) continue;
            // Ensure the line length matches column (allow shorter lines, fill with '-')
            for(int c = 0; c < column && c < static_cast<int>(line.size()); c++) {
                char ch = line[c];
                stage.tileMap[r][c] = ch;
                if(ch == 'P') {
                    // Player start tile position in tile coordinates (c,r)
                    stage.getPlayer().posX = c;
                    stage.getPlayer().posY = r;
                }
            }
            // Fill remaining columns with open space if line shorter
            for(int c = static_cast<int>(line.size()); c < column; ++c) {
                stage.tileMap[r][c] = '-';
            }
            r++;
            if(r >= row) {
                // consume remaining lines up to MAP_END
                while(std::getline(file, line) && line != "MAP_END");
                break;
            }
        }

        // Parse pattern section
        std::string dispensorPattern;
        std::string guardMonsterPattern;

        // Find PATTERN_START
        while(std::getline(file, line) && line != "PATTERN_START") {
            if(line == "STAGE_END") break; // no patterns
        }
        if(line == "PATTERN_START") {
            // Read until PATTERN_END
            while(std::getline(file, line) && line != "PATTERN_END") {
                if(line.rfind("DISPENSOR:", 0) == 0) {
                    dispensorPattern = line.substr(std::string("DISPENSOR:").size());
                } else if(line.rfind("GUARD_MONSTER:", 0) == 0) {
                    guardMonsterPattern = line.substr(std::string("GUARD_MONSTER:").size());
                }
            }
        }

        // Finally, advance to STAGE_END
        while(line != "STAGE_END" && std::getline(file, line)) {}

        // Store patterns into stage
        stage.setPatternDispensor(dispensorPattern);
        stage.setPatternTraceMonster(guardMonsterPattern);
        Logger::log("Stage " + std::to_string(stageId) + " loaded from file.");
        stage.print();

        // Add stage to stages vector
        stages.emplace_back(std::move(stage));
    }

    file.close();
}

int Stage::getRow() const { return row; }
int Stage::getColumn() const { return column; }
Player& Stage::getPlayer() { return player; }

void Stage::setPatternDispensor(const std::string& pattern) {
    patternDispensor = pattern;
}

void Stage::setPatternTraceMonster(const std::string& pattern) {
    patternTraceMonster = pattern;
}

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
    Logger::log_debug("Printing tile map for Stage " + std::to_string(stageId) + ":");
    Logger::log_debug("Size (" + std::to_string(column) + ", " + std::to_string(row) + "):");
    Logger::log_debug("=====================");
    for(const auto& row : tileMap) {
        std::string line;
        for(const auto& tile : row) {
            line += tile;
        }
        Logger::log_debug(line);
    }
    Logger::log_debug("=====================");
    Logger::log_debug("Pattern Dispensor: " + patternDispensor);
    Logger::log_debug("Pattern Trace Monster: " + patternTraceMonster);
    Logger::log_debug("=====================");
}