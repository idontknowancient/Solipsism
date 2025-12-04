#include "Constants.hpp"
#include "Utils.hpp"
#include "Logger.hpp"
#include "Stage.hpp"
#include <iostream>
#include <vector>

Entity::Entity(const sf::Texture& texture) : sprite(texture) {}

sf::Sprite& Entity::getSprite() {
    return sprite;
}

void Entity::draw(sf::RenderWindow& window, int tile_size) {
    window.draw(sprite);
}

Player::Player(const sf::Texture& texture) : Entity(texture) {
    Logger::log("Player created.");
}

void Player::interact(Entity& entity) {
    // To be determined
}



// ========== Stage Class =============
Stage::Stage(int stageId, int column, int row) : stageId(stageId), row(row), column(column), player(Resource::getPlayerTexture()) {
    // Initialize tile map with open space '-'
    tileMap.resize(row, std::vector<char>(column, '-'));
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
                    stage.getPlayer().posTile = {c, r};
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
        stage.createTiles(100);
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

void Stage::createTiles(int tile_size) {
    this->tile_size = tile_size;
    float start_x = (WORLD_WIDTH - (column * tile_size)) / 2.f;
    float start_y = (WORLD_HEIGHT - (row * tile_size)) / 2.f;

    // Scale player sprite to fit tile size
    sf::Sprite& playerSprite = player.getSprite();
    playerSprite.scale({static_cast<float>(tile_size) / playerSprite.getTexture().getSize().x, 
        static_cast<float>(tile_size) / playerSprite.getTexture().getSize().y});
    player.posWindow = {start_x + player.posTile.x * tile_size, start_y + player.posTile.y * tile_size};
    playerSprite.setPosition(player.posWindow);
    Logger::log("Player positioned at (" 
        + std::to_string(player.posWindow.x) + ", " 
        + std::to_string(player.posWindow.y) + ").");

    for(int i = 0; i < row; i++) {
        for(int j = 0; j < column; j++) {
            // Use new to allocate on heap to prevent going out of scope
            sf::RectangleShape* tile = new sf::RectangleShape(sf::Vector2f(tile_size, tile_size));
            
            // Set position
            float x = start_x + j * tile_size;
            float y = start_y + i * tile_size;
            tile->setPosition({x, y});

            // Set tile appearance
            tile->setOutlineThickness(1.0f);
            tile->setOutlineColor(sf::Color(100, 100, 100));
            
            // Checkerboard colors
            char tileType = tileMap[i][j];
            switch(tileType) {
                case 'P': // Player start
                    tile->setFillColor(TILE_COLOR_PLAYER);
                    break;
                case 'G': // Goal
                    tile->setFillColor(TILE_COLOR_GOAL);
                    break;
                case 'X': // Wall
                    tile->setFillColor(TILE_COLOR_WALL);
                    break;
                case 'D': // Dispensor
                    tile->setFillColor(TILE_COLOR_DISPENSOR);
                    break;
                case 'M': // Trace Monster
                    tile->setFillColor(TILE_COLOR_TRACE_MONSTER);
                    break;
                case 'm': // Guard Monster
                    tile->setFillColor(TILE_COLOR_GUARD_MONSTER);
                    break;
                case '-': // Open space
                default:
                    tile->setFillColor(TILE_COLOR_NORMAL);
                    break;
            }
            
            shapes.push_back(tile);
            Logger::log("Created tile at (" + std::to_string(j) + ", " + std::to_string(i) + ") of type '" 
                + tileType + "' at position (" + std::to_string(x) + ", " + std::to_string(y) + ").");
        }
    }
}

bool Stage::isValidMove(Entity& entity, std::string direction) {
    sf::Vector2i newPos = entity.posTile;
    if(direction == "W") {
        newPos.y -= 1;
    } else if(direction == "S") {
        newPos.y += 1;
    } else if(direction == "A") {
        newPos.x -= 1;
    } else if(direction == "D") {
        newPos.x += 1;
    } else {
        Logger::log("Invalid direction input: " + direction);
        return false; // Invalid direction
    }

    // Check bounds
    if(newPos.x < 0 || newPos.x >= column || newPos.y < 0 || newPos.y >= row) {
        Logger::log("Move out of bounds to (" + std::to_string(newPos.x) + ", " + std::to_string(newPos.y) + ").");
        return false;
    }

    // Check tile type
    char tileType = tileMap[newPos.y][newPos.x];
    if(tileType == 'X') { // Wall
        Logger::log("Move blocked by wall at (" + std::to_string(newPos.x) + ", " + std::to_string(newPos.y) + ").");
        return false;
    }

    return true; // Valid move
}

bool Stage::moveEntitySuccessful(Entity& entity, std::string direction) {
    if(!isValidMove(entity, direction)) return false;

    // Update tile and window position
    if(direction == "W") {
        entity.posTile.y -= 1;
        entity.posWindow.y -= tile_size;
    } else if(direction == "S") {
        entity.posTile.y += 1;
        entity.posWindow.y += tile_size;
    } else if(direction == "A") {
        entity.posTile.x -= 1;
        entity.posWindow.x -= tile_size;
    } else if(direction == "D") {
        entity.posTile.x += 1;
        entity.posWindow.x += tile_size;
    }
    entity.getSprite().setPosition(entity.posWindow);

    Logger::log("Entity moved to (" 
        + std::to_string(entity.posTile.x) + ", " 
        + std::to_string(entity.posTile.y) + ").");
    return true;
}

void Stage::draw(sf::RenderWindow& window) {
    // Draw all shapes
    for(const auto& shape : shapes) {
        window.draw(*shape);
    }

    // Draw player
    player.draw(window, tile_size);
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