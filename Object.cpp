#include "Logger.hpp"
#include "Utils.hpp"
#include "Astar.hpp"
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


bool Object::isValidAction(std::vector<std::vector<char>>& tileMap, sf::Vector2i newPosTile) {
    int column = tileMap[0].size();
    int row = tileMap.size();

    // Check bounds
    if(newPosTile.x < 0 || newPosTile.x >= column || newPosTile.y < 0 || newPosTile.y >= row) {
        Logger::log_debug("Action out of bounds to (" + std::to_string(newPosTile.x) + ", " + std::to_string(newPosTile.y) + ").");
        return false;
    }

    // Check tile type
    char tileType = tileMap[newPosTile.y][newPosTile.x];
    if(tileType == 'X') { // Wall
        Logger::log_debug("Action blocked by wall at (" + std::to_string(newPosTile.x) + ", " + std::to_string(newPosTile.y) + ").");
        return false;
    } else if(tileType == 'D') { // Dispenser
        Logger::log_debug("Action blocked by dispenser at (" + std::to_string(newPosTile.x) + ", " + std::to_string(newPosTile.y) + ").");
        return false;
    }

    return true; // Valid action
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

    return Object::isValidAction(tileMap, newPos);
}

void Player::update(std::vector<std::vector<char>>& tileMap, int tile_size, const Action& action) {
    Logger::log_debug("Updating Player.");
    if(!isValidMove(tileMap, action)) return;

    // Update tile and window position
    if(action == Action::MoveUp) {
        posTile.y -= 1;
        posWindow.y -= tile_size;
        tileMap[posTile.y + 1][posTile.x] = '-';
        tileMap[posTile.y][posTile.x] = 'P';
    } else if(action == Action::MoveDown) {
        posTile.y += 1;
        posWindow.y += tile_size;
        tileMap[posTile.y - 1][posTile.x] = '-';
        tileMap[posTile.y][posTile.x] = 'P';
    } else if(action == Action::MoveLeft) {
        posTile.x -= 1;
        posWindow.x -= tile_size;
        tileMap[posTile.y][posTile.x + 1] = '-';
        tileMap[posTile.y][posTile.x] = 'P';
    } else if(action == Action::MoveRight) {
        posTile.x += 1;
        posWindow.x += tile_size;
        tileMap[posTile.y][posTile.x - 1] = '-';
        tileMap[posTile.y][posTile.x] = 'P';
    }
    getSprite().setPosition(posWindow);

    Logger::log_debug("Player moved to (" 
        + std::to_string(posTile.x) + ", " 
        + std::to_string(posTile.y) + ").");
}



// ========= Wall and Goal Class =============
Wall::Wall(sf::Vector2i posTile, sf::Vector2f posWindow, int tile_size) : 
    Object(Resource::getWallTexture(), posTile, posWindow, tile_size) {
    Logger::log("Wall created at tile (" 
        + std::to_string(posTile.x) + ", " + std::to_string(posTile.y) + ").");
}



Goal::Goal(sf::Vector2i posTile, sf::Vector2f posWindow, int tile_size) : 
    Object(Resource::getGoalTexture(), posTile, posWindow, tile_size) {
    Logger::log("Goal created at tile (" 
        + std::to_string(posTile.x) + ", " + std::to_string(posTile.y) + ").");
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

    return Object::isValidAction(tileMap, newPos);
}

TraceMonster::TraceMonster(sf::Vector2i posTile, sf::Vector2f posWindow, int tile_size) : 
    Monster(Resource::getTraceMonsterTexture(), posTile, posWindow, tile_size) {
    Logger::log("TraceMonster created at tile (" 
        + std::to_string(posTile.x) + ", " + std::to_string(posTile.y) + ").");
}

void TraceMonster::update(std::vector<std::vector<char>>& tileMap, int tile_size, const sf::Vector2i& playerPosTile) {
    Logger::log_debug("Updating TraceMonster at (" 
        + std::to_string(posTile.x) + ", " + std::to_string(posTile.y) + ").");
    
    // 1. 執行 A* 尋路
    Pathfinder pathfinder;
    std::vector<sf::Vector2i> path = pathfinder.findPath(posTile, playerPosTile, tileMap);
    
    // 檢查路徑是否有效，且長度大於 1 (至少包含起點和一個移動點)
    if (path.size() > 1) {
        
        sf::Vector2i nextTilePos = path[1]; // 獲取下一步的最佳網格座標
        char nextTileChar = tileMap[nextTilePos.y][nextTilePos.x];

        if(nextTileChar == 'M' || nextTileChar == 'm') {
            Logger::log_debug("TraceMonster next position blocked by other Monster at (" 
                + std::to_string(nextTilePos.x) + ", " + std::to_string(nextTilePos.y) + ").");
            return; // 被其他 TraceMonster 阻擋，無法移動
        }

        // 2. 清除舊位置符號
        // 將怪物舊的網格位置標記為空位 (假設'-'是空位)
        tileMap[posTile.y][posTile.x] = '-'; 
        
        // 3. 更新網格位置和視窗位置
        
        // 計算位移量 (以 tile_size 為單位)
        float deltaX = (float)(nextTilePos.x - posTile.x);
        float deltaY = (float)(nextTilePos.y - posTile.y);
        
        // 更新網格座標
        posTile = nextTilePos; 
        
        // 更新視窗像素座標
        posWindow.x += deltaX * tile_size;
        posWindow.y += deltaY * tile_size;
        
        // 4. 在新位置標記怪物
        // 假設 'M' 是 TraceMonster 的符號
        tileMap[posTile.y][posTile.x] = 'M'; 
        
        // 5. 將 Sprite 繪圖位置同步到新的視窗座標
        getSprite().setPosition(posWindow);

        Logger::log_debug("TraceMonster moved to (" + std::to_string(posTile.x) + ", " + std::to_string(posTile.y) + ").");
    } else {
        Logger::log_debug("TraceMonster is blocked or at goal.");
        // 如果 path.size() <= 1，表示怪物已被包圍或已到達目標，原地不動。
    }
}

GuardMonster::GuardMonster(sf::Vector2i posTile, sf::Vector2f posWindow, int tile_size, const std::string& pattern) : 
    Monster(Resource::getGuardMonsterTexture(), posTile, posWindow, tile_size), behaviorPattern(pattern) {
    Logger::log("GuardMonster created at tile (" 
        + std::to_string(posTile.x) + ", " + std::to_string(posTile.y) + ") "
        + "with behavior pattern: " + behaviorPattern);
}

void GuardMonster::update(std::vector<std::vector<char>>& tileMap, int tile_size) {
    Logger::log_debug("Updating GuardMonster at (" 
        + std::to_string(posTile.x) + ", " + std::to_string(posTile.y) + ").");
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



// ========== Dispenser Class =============
Dispenser::Dispenser(sf::Vector2i posTile, sf::Vector2f posWindow, int tile_size, const std::string& pattern) : 
    Object(Resource::getDispenserTexture(), posTile, posWindow, tile_size), behaviorPattern(pattern) {
    Logger::log("Dispenser created at tile (" 
        + std::to_string(posTile.x) + ", " + std::to_string(posTile.y) + ").");
}

bool Dispenser::isSpawnable(std::vector<std::vector<char>>& tileMap, char actionChar) {
    sf::Vector2i newPos = posTile;
    if(actionChar == 'U') {
        newPos.y -= 1;
    } else if(actionChar == 'D') {
        newPos.y += 1;
    } else if(actionChar == 'L') {
        newPos.x -= 1;
    } else if(actionChar == 'R') {
        newPos.x += 1;
    } else {
        return false; // Invalid actionChar
    }

    return Object::isValidAction(tileMap, newPos) && tileMap[newPos.y][newPos.x] == '-';
}

void Dispenser::update(std::vector<std::vector<char>>& tileMap, int tile_size, std::vector<std::unique_ptr<Object>>& bufferObjects) {
    Logger::log_debug("Updating Dispenser at (" 
        + std::to_string(posTile.x) + ", " + std::to_string(posTile.y) + ").");
    // Dispenser does not move, but it will project arrows based on its behavior pattern.
    if(behaviorPattern.empty()) return;

    char actionChar = behaviorPattern[0];
    cyclePattern(behaviorPattern);
    if(!isSpawnable(tileMap, actionChar)) return;

    sf::Vector2i arrowPosTile = posTile;
    sf::Vector2f arrowPosWindow = posWindow;

    // Create a new Arrow object based on the actionChar
    if(actionChar == 'U') {
        arrowPosTile.y -= 1;
        arrowPosWindow.y -= tile_size;
    } else if(actionChar == 'D') {
        arrowPosTile.y += 1;
        arrowPosWindow.y += tile_size;
    } else if(actionChar == 'L') {
        arrowPosTile.x -= 1;
        arrowPosWindow.x -= tile_size;
    } else if(actionChar == 'R') {
        arrowPosTile.x += 1;
        arrowPosWindow.x += tile_size;
    }

    // Mark arrow position in tile map
    tileMap[arrowPosTile.y][arrowPosTile.x] = 'A';
    bufferObjects.emplace_back(std::make_unique<Arrow>(arrowPosTile, arrowPosWindow, tile_size, actionChar));
    Logger::log("Dispenser at (" 
        + std::to_string(posTile.x) + ", " + std::to_string(posTile.y) 
        + ") dispensed an Arrow.");
}

std::string& Dispenser::getBehaviorPattern() {
    return behaviorPattern;
}



// ========== Projectile Class =============
sf::Vector2i Projectile::getOriginalPosTile() const {
    return originalPosTile;
}

Arrow::Arrow(sf::Vector2i posTile, sf::Vector2f posWindow, int tile_size, char direction) : 
    Projectile(Resource::getArrowTexture(), posTile, posWindow, tile_size, direction) {
    Logger::log("Arrow created at tile (" 
        + std::to_string(posTile.x) + ", " + std::to_string(posTile.y) + ").");
}

void Arrow::update(std::vector<std::vector<char>>& tileMap, int tile_size) {
    Logger::log_debug("Updating Arrow at (" 
        + std::to_string(posTile.x) + ", " + std::to_string(posTile.y) + ").");
    
    sf::Vector2i newPosTile = posTile;
    sf::Vector2f newPosWindow = posWindow;
    
    originalPosTile = posTile;
    
    // Move based on direction
    if(direction == 'U') {
        newPosTile.y -= 1;
        newPosWindow.y -= tile_size;
    } else if(direction == 'D') {
        newPosTile.y += 1;
        newPosWindow.y += tile_size;
    } else if(direction == 'L') {
        newPosTile.x -= 1;
        newPosWindow.x -= tile_size;
    } else if(direction == 'R') {
        newPosTile.x += 1;
        newPosWindow.x += tile_size;
    }
    
    // Invalid move, let caller remove this arrow
    if(!Object::isValidAction(tileMap, newPosTile)) return;
    
    tileMap[originalPosTile.y][originalPosTile.x] = '-';

    posTile = newPosTile;
    posWindow = newPosWindow;
    tileMap[posTile.y][posTile.x] = 'A';
    getSprite().setPosition(posWindow);
    
    Logger::log_debug("Arrow moved to (" + std::to_string(posTile.x) + ", " + std::to_string(posTile.y) + ").");
}