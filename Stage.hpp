#pragma once
#include <SFML/Graphics.hpp>
#include "Utils.hpp"
#include "Logger.hpp"
#include <iostream>
#include <vector>
#include <string>

class Entity {
protected:
    sf::Sprite sprite;

public:
    // Position in tile coordinates
    sf::Vector2i posTile;
    // Position in window coordinates
    sf::Vector2f posWindow;

    Entity(const sf::Texture& texture);
    virtual ~Entity() = default;

    sf::Sprite& getSprite();

    virtual void interact(Entity& entity) = 0;

    void draw(sf::RenderWindow& window, int tile_size);
};

class Player : public Entity {
public:
    Player(const sf::Texture& texture);
    void interact(Entity& entity) override;
};

class Monster : public Entity {
    
};

class Projectile : public Entity {
    
};

class Trap {

};



// ========== Stage Class =============
class Stage {
private:
    // Starts from 1
    int stageId;
    int row;
    int column;
    // Hold tile data
    int tile_size;
    std::vector<std::vector<char>> tileMap;
    // Store all shapes drawn
    // Later binding
    std::vector<sf::Shape*> shapes;
    // Store monsters and projectiles in the stage
    std::vector<Entity> entities;
    Player player;

    std::string patternDispensor;
    std::string patternTraceMonster;

public:
    Stage(int stageId, int column, int row);

    ~Stage();

    static void createFromFile(std::vector<Stage>& stages);

    // Disable copy to avoid double-free of raw shape pointers
    Stage(const Stage&) = delete;
    Stage& operator=(const Stage&) = delete;

    // Allow move (default)
    Stage(Stage&&) = default;
    Stage& operator=(Stage&&) = default;

    int getRow() const;
    int getColumn() const;
    Player& getPlayer();

    void setPatternDispensor(const std::string& pattern);
    void setPatternTraceMonster(const std::string& pattern);

    void createTiles(int tile_size);

    // Attempt to move the entity in the specified direction
    // Returns true if the move was successful, false otherwise
    bool moveEntitySuccessful(Entity& entity, std::string direction);

    void draw(sf::RenderWindow& window);
    void print() const;
};