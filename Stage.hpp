#pragma once
#include <SFML/Graphics.hpp>
#include "Utils.hpp"
#include "Logger.hpp"
#include "Object.hpp"
#include <iostream>
#include <vector>
#include <memory>
#include <string>
#include <queue>

class Stage {
private:
    // Starts from 1
    int stageId;
    int row;
    int column;
    int actionPerTurn;

    // Hold tile data
    int tile_size;
    // Start position of the tile map in window coordinates
    float start_x;
    float start_y;

    // 2D tile map representation
    std::vector<std::vector<char>> tileMap;
    // Store all shapes drawn
    // Use unique_ptr to manage shape memory automatically
    std::vector<std::unique_ptr<sf::Shape>> shapes;

    // Store monsters / projectiles / traps in the stage
    std::vector<std::unique_ptr<Object>> objects;
    std::unique_ptr<Player> player;
    // Record actions by player
    std::queue<Action> actions;

    // Behavior patterns
    std::string patternDispensor;
    std::string patternGuardMonster;

    void handleObjectAction();
    void handlePlayerAction();

public:
    Stage(int stageId, int column, int row, int actionPerTurn);
    // Disable copy to avoid double-free of raw shape pointers
    Stage(const Stage&) = delete;
    Stage& operator=(const Stage&) = delete;
    // Allow move
    Stage(Stage&&) = default;
    Stage& operator=(Stage&&) = default;

    int getRow() const;
    int getColumn() const;
    Player& getPlayer();

    void setPatternDispensor(const std::string& pattern);
    void setPatternGuardMonster(const std::string& pattern);

    static void createFromFile(std::vector<Stage>& stages);
    void createTiles(int tile_size);

    void addAction(const Action action);
    bool reachMaxActions() const;
    void undoLastAction();

    // Advance by actionPerTurn actions
    void advance();

    void draw(sf::RenderWindow& window);
    void print() const;
};