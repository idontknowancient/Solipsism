#include "Constants.hpp"
#include "Utils.hpp"
#include "Logger.hpp"
#include "Stage.hpp"
#include "Object.hpp"
#include <iostream>
#include <vector>

Stage::Stage(int stageId, int column, int row, int actionPerTurn) : stageId(stageId), row(row), column(column), actionPerTurn(actionPerTurn), tile_size(100) {
    // Initialize start positions for the tile map in window coordinates
    this->start_x = (WORLD_WIDTH - (column * tile_size)) / 2.f;
    this->start_y = (WORLD_HEIGHT - (row * tile_size)) / 2.f;
    
    // Initialize tile map with open space '-'
    tileMap.resize(row, std::vector<char>(column, '-'));
    
    // Create visual tiles for the stage (will be called after loading map from file)
    // createTiles(100) should be called after map is loaded
    
    Logger::log("Stage " + std::to_string(stageId) + " created with size (" 
        + std::to_string(column) + ", " + std::to_string(row) + ").");
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
        int actionPerTurn = 1;

        // Parse header lines: STAGE_ID, COLUMN, ROW
        while(std::getline(file, line)) {
            if(line.rfind("STAGE_ID:", 0) == 0) {
                stageId = std::stoi(line.substr(std::string("STAGE_ID:").size()));
                Logger::log("Loading Stage: " + std::to_string(stageId));
            } else if(line.rfind("COLUMN:", 0) == 0) {
                column = std::stoi(line.substr(std::string("COLUMN:").size()));
            } else if(line.rfind("ROW:", 0) == 0) {
                row = std::stoi(line.substr(std::string("ROW:").size()));
            } else if(line.rfind("ACTION_PER_TURN:", 0) == 0) {
                actionPerTurn = std::stoi(line.substr(std::string("ACTION_PER_TURN:").size()));
            } else if(line == "PATTERN_START") {
                break; // Proceed to pattern parsing
            }
        }

        if(stageId <= 0 || column <= 0 || row <= 0) {
            Logger::log("Invalid stage header encountered. Skipping.");
            // Skip to next STAGE_END
            while(std::getline(file, line) && line != "STAGE_END");
            continue;
        }

        Stage stage(stageId, column, row, actionPerTurn);

        // Parse pattern section
        std::string guardMonsterPattern;
        std::string dispenserPattern;
        std::string slicedPattern;

        if(line == "PATTERN_START") {
            Logger::log_debug("Parsing patterns for stage " + std::to_string(stageId));
            // Read until PATTERN_END
            while(std::getline(file, line) && line != "PATTERN_END") {
                line = trim(line);
                if(line.rfind("DISPENSER:", 0) == 0) {
                    dispenserPattern = line.substr(std::string("DISPENSER:").size());
                    // Remove leading/trailing whitespace
                    dispenserPattern = trim(dispenserPattern);
                } else if(line.rfind("GUARD_MONSTER:", 0) == 0) {
                    guardMonsterPattern = line.substr(std::string("GUARD_MONSTER:").size());
                    // e.g. U2D2L4 -> UUDDLLLL
                    guardMonsterPattern = processPattern(guardMonsterPattern);
                    // Remove leading/trailing whitespace
                    guardMonsterPattern = trim(guardMonsterPattern);
                }
            }
        }
        // Store patterns into stage
        stage.setPatternGuardMonster(guardMonsterPattern);
        stage.setPatternDispenser(dispenserPattern);

        // Find MAP_START
        while(std::getline(file, line) && line != "MAP_START") {
            if(line == "STAGE_END") {
                Logger::log_debug("Warning: MAP_START not found for stage " + std::to_string(stageId));
                break;
            }
        }

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
                // Initial position in window coordinates
                sf::Vector2f posWindow = {stage.start_x + c * stage.tile_size, stage.start_y + r * stage.tile_size};

                // Object creation based on symbol
                // Player
                if(ch == SYMBOL_PLAYER) {
                    stage.player = std::make_unique<Player>(sf::Vector2i{c, r}, posWindow, stage.tile_size);
                    stage.initialPlayer = std::make_unique<Player>(sf::Vector2i{c, r}, posWindow, stage.tile_size);
                }

                // Trace monsters 
                else if(ch == SYMBOL_TRACE_MONSTER) {
                    stage.objects.emplace_back(std::make_unique<TraceMonster>(
                        sf::Vector2i{c, r}, posWindow, stage.tile_size));
                    stage.initialObjects.emplace_back(std::make_unique<TraceMonster>(
                        sf::Vector2i{c, r}, posWindow, stage.tile_size));
                }

                // Guard monsters 
                else if(ch == SYMBOL_GUARD_MONSTER) {
                    // Extract the first pattern segment (from start to first semicolon)
                    int semicolonPos = guardMonsterPattern.find(';');
                    if(semicolonPos != std::string::npos) {
                        slicedPattern = guardMonsterPattern.substr(0, semicolonPos);
                        guardMonsterPattern = guardMonsterPattern.substr(semicolonPos + 1);
                    } else {
                        slicedPattern = guardMonsterPattern;
                    }
                    stage.objects.emplace_back(std::make_unique<GuardMonster>(
                        sf::Vector2i{c, r}, posWindow, stage.tile_size, slicedPattern));
                    stage.initialObjects.emplace_back(std::make_unique<GuardMonster>(
                        sf::Vector2i{c, r}, posWindow, stage.tile_size, slicedPattern));
                }

                // Dispensers
                else if(ch == SYMBOL_DISPENSER) {
                    // Extract the first pattern segment (from start to first semicolon)
                    int semicolonPos = dispenserPattern.find(';');
                    if(semicolonPos != std::string::npos) {
                        slicedPattern = dispenserPattern.substr(0, semicolonPos);
                        dispenserPattern = dispenserPattern.substr(semicolonPos + 1);
                    } else {
                        slicedPattern = dispenserPattern;
                    }
                    stage.objects.emplace_back(std::make_unique<Dispenser>(
                        sf::Vector2i{c, r}, posWindow, stage.tile_size, slicedPattern));
                    stage.initialObjects.emplace_back(std::make_unique<Dispenser>(
                        sf::Vector2i{c, r}, posWindow, stage.tile_size, slicedPattern));
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

        // Save initial state for reset
        stage.initialTileMap = stage.tileMap;

        // Handle if symbol of player not found
        if(!stage.player) {
            Logger::log("Warning: Player symbol not found in stage " + std::to_string(stageId) + ". Creating default player at (0,0).");
            stage.player = std::make_unique<Player>(sf::Vector2i{0, 0}, 
                sf::Vector2f{stage.start_x, stage.start_y}, stage.tile_size);
            stage.initialPlayer = std::make_unique<Player>(sf::Vector2i{0, 0}, 
                sf::Vector2f{stage.start_x, stage.start_y}, stage.tile_size);
        }

        // Advance to STAGE_END
        while(line != "STAGE_END" && std::getline(file, line)) {}

        stage.createTiles(stage.tile_size);
        Logger::log("Total objects in stage " + std::to_string(stageId) + ": " + std::to_string(stage.objects.size()));
        Logger::log("Stage " + std::to_string(stageId) + " loaded from file.");
        stage.print();

        // Add stage to stages vector
        stages.emplace_back(std::move(stage));
    }

    file.close();
}

void Stage::createTiles(int tile_size) {
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
                case SYMBOL_PLAYER:
                    tile->setFillColor(TILE_COLOR_PLAYER);
                    break;
                case SYMBOL_GOAL:
                    tile->setFillColor(TILE_COLOR_GOAL);
                    break;
                case SYMBOL_WALL:
                    tile->setFillColor(TILE_COLOR_WALL);
                    break;
                case SYMBOL_DISPENSER:
                    tile->setFillColor(TILE_COLOR_DISPENSER);
                    break;
                case SYMBOL_TRACE_MONSTER:
                    tile->setFillColor(TILE_COLOR_TRACE_MONSTER);
                    break;
                case SYMBOL_GUARD_MONSTER:
                    tile->setFillColor(TILE_COLOR_GUARD_MONSTER);
                    break;
                case SYMBOL_OPEN_SPACE:
                default:
                    tile->setFillColor(TILE_COLOR_NORMAL);
                    break;
            }
            
            shapes.emplace_back(std::unique_ptr<sf::Shape>(tile));
            Logger::log_debug("Created tile at (" + std::to_string(j) + ", " + std::to_string(i) + ") of type '" 
                + tileType + "' at position (" + std::to_string(x) + ", " + std::to_string(y) + ").");
        }
    }
}

int Stage::getRow() const { return row; }
int Stage::getColumn() const { return column; }
Player& Stage::getPlayer() { return *player; }

void Stage::setPatternDispenser(const std::string& pattern) {
    patternDispenser = pattern;
}

void Stage::setPatternGuardMonster(const std::string& pattern) {
    patternGuardMonster = pattern;
}

void Stage::addAction(const Action action) {
    actions.push(action);
}

bool Stage::reachMaxActions() const {
    return actions.size() >= static_cast<size_t>(actionPerTurn);
}

void Stage::undoLastAction() {
    if(actions.empty()) {
        Logger::log_debug("No actions to undo.");
        return;
    }

    // Remove the last action added
    std::queue<Action> tempQueue;
    int actionCount = actions.size();
    for(int i = 0; i < actionCount - 1; i++) {
        tempQueue.push(actions.front());
        actions.pop();
    }
    actions.pop(); // Remove the last action

    // Restore the remaining actions back to the original queue
    actions = std::move(tempQueue);

    Logger::log_debug("Last action undone. Remaining actions: " + std::to_string(actions.size()));
}

void Stage::handleObjectAction() {
    Logger::log("Handling object action.");
    
    // First handle projectiles
    for(int i = 0; i < objects.size(); i++) {
        std::unique_ptr<Object>& object = objects[i];

        if(Arrow* arrow = dynamic_cast<Arrow*>(object.get())) {
            // Store old position before update
            sf::Vector2i oldPosTile = arrow->posTile;
            
            // Update arrow
            arrow->update(tileMap, tile_size);
            if(shouldRemoveProjectile(arrow, arrow->getOriginalPosTile(), i)) {
                objectsToRemove.push_back(i);
            }
        }
    }
    // Then handle other objects
    for(int i = 0; i < objects.size(); i++) {
        std::unique_ptr<Object>& object = objects[i];
        
        // Use .get() to access the raw pointer from unique_ptr
        if(TraceMonster* traceMonster = dynamic_cast<TraceMonster*>(object.get())) {
            traceMonster->update(tileMap, tile_size, player->posTile);
        } else if(GuardMonster* guardMonster = dynamic_cast<GuardMonster*>(object.get())) {
            guardMonster->update(tileMap, tile_size);
        } else if(Dispenser* dispenser = dynamic_cast<Dispenser*>(object.get())) {
            dispenser->update(tileMap, tile_size, bufferObjects);
        }
    }

    // Add buffered objects to main objects vector
    for(auto& bufferedObject : bufferObjects) {
        objects.emplace_back(std::move(bufferedObject));
    }
    bufferObjects.clear();
    
    // Remove projectiles in reverse order to avoid index shifting issues
    for(int i = objectsToRemove.size() - 1; i >= 0; i--) {
        int removeIndex = objectsToRemove[i];
        objects.erase(objects.begin() + removeIndex);
        Logger::log_debug("Removed arrow at index " + std::to_string(removeIndex));
    }
    objectsToRemove.clear();

    Logger::log_debug("Remaining objects after handling actions: " + std::to_string(objects.size()));
}

void Stage::handlePlayerAction() {
    Logger::log("Handling player action.");
    Action action = actions.front();
    actions.pop();

    switch(action) {
        case Action::MoveUp:
        case Action::MoveDown:
        case Action::MoveLeft:
        case Action::MoveRight:
            player->update(tileMap, tile_size, action);
            break;
        case Action::Attack:
            // To be implemented
            break;
        case Action::None:
            break;
    }
}

bool Stage::shouldRemoveProjectile(Projectile* projectile, sf::Vector2i oldPosTile, int i) {
    // If projectile position didn't change, it hit a wall or obstacle
    if(projectile->posTile == oldPosTile) {
        tileMap[projectile->posTile.y][projectile->posTile.x] = '-';
        Logger::log_debug("Arrow at (" + std::to_string(oldPosTile.x) + ", " + std::to_string(oldPosTile.y) + ") stopped and will be removed.");
        return true;
    } 

    return false;
}

void Stage::advance() {
    Logger::log("Advancing stage by " + std::to_string(actionPerTurn) + " actions.");
    for(int i = 0; i < actionPerTurn; i++) {
        if(actions.empty()) {
            Logger::log_debug("No more actions to handle.");
            break;
        }
        handleObjectAction();
        handlePlayerAction();
    }
    Logger::log_debug("Stage advanced.");
    Logger::log_debug("Stage state after advance:");
    print();
}

void Stage::draw(sf::RenderWindow& window) {
    // Draw all shapes
    for(const auto& shape : shapes) {
        window.draw(*shape);
    }

    //Draw all objects
    for(auto& object : objects) {
        object->draw(window, tile_size);
    }

    // Draw player
    player->draw(window, tile_size);

}

void Stage::print() const {
    Logger::log_debug("=====================");
    Logger::log_debug("Printing tile map for Stage " + std::to_string(stageId) + ":");
    Logger::log_debug("Size (" + std::to_string(column) + ", " + std::to_string(row) + "):");
    Logger::log_debug("Action Per Turn: " + std::to_string(actionPerTurn));
    Logger::log_debug("=====================");
    for(const auto& row : tileMap) {
        std::string line;
        for(const auto& tile : row) {
            line += tile;
        }
        Logger::log_debug(line);
    }
    Logger::log_debug("=====================");
    Logger::log_debug("Pattern Guard Monster: " + patternGuardMonster);
    Logger::log_debug("Pattern Dispenser: " + patternDispenser);
    Logger::log_debug("=====================");
}

void Stage::reset() {
    Logger::log("Resetting stage " + std::to_string(stageId) + " to initial state.");

    // Clear any queued actions
    while(!actions.empty()) actions.pop();

    // Restore tile map
    if(!initialTileMap.empty()) {
        tileMap = initialTileMap;
    } else {
        Logger::log_debug("No initial tileMap stored; skipping restore.");
    }

    // Clear current objects and shapes
    objects.clear();
    shapes.clear();

    // Restore player from initial state
    if(initialPlayer) {
        player = std::make_unique<Player>(initialPlayer->posTile, initialPlayer->posWindow, tile_size);
        Logger::log("Player reset to initial position (" + std::to_string(initialPlayer->posTile.x) + ", " + std::to_string(initialPlayer->posTile.y) + ").");
    } else {
        // If no initial player was stored, create default
        player = std::make_unique<Player>(sf::Vector2i{0,0}, sf::Vector2f{start_x, start_y}, tile_size);
        Logger::log_debug("No initial player found; created default player at (0,0).");
    }

    // Clone all initial objects back to objects
    for(const auto& objPtr : initialObjects) {
        if(TraceMonster* tm = dynamic_cast<TraceMonster*>(objPtr.get())) {
            objects.emplace_back(std::make_unique<TraceMonster>(
                tm->posTile, tm->posWindow, tile_size));
        } else if(GuardMonster* gm = dynamic_cast<GuardMonster*>(objPtr.get())) {
            objects.emplace_back(std::make_unique<GuardMonster>(
                gm->posTile, gm->posWindow, tile_size, gm->getBehaviorPattern()));
        } else if(Dispenser* disp = dynamic_cast<Dispenser*>(objPtr.get())) {
            objects.emplace_back(std::make_unique<Dispenser>(
                disp->posTile, disp->posWindow, tile_size, disp->getBehaviorPattern()));
        }
    }

    // Recreate visual tiles
    createTiles(tile_size);

    Logger::log("Stage " + std::to_string(stageId) + " reset complete.");
    print();
}