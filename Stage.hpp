#pragma once
#include <SFML/Graphics.hpp>
#include "Utils.hpp"
#include "Logger.hpp"
#include <iostream>
#include <vector>

class Entity {
protected:
    sf::Sprite sprite;

public:
    // Position in tile coordinates
    int posX;
    int posY;

    Entity(int x, int y, const sf::Texture& texture);
    virtual ~Entity() = default;

    virtual void interact(Entity& entity) = 0;

    void draw(sf::RenderWindow& window);
};

class Player : public Entity {
public:
    Player(int x, int y, const sf::Texture& texture);
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
    int stageNum;
    int row;
    int column;
    // Hold tile data
    std::vector<std::vector<char>> tileMap; 
    // Store all shapes drawn
    // Later binding
    std::vector<sf::Shape*> shapes;
    // Store monsters and projectiles in the stage
    std::vector<Entity> entities;
    Player player;

public:
    Stage(int stageNum, int column, int row);

    ~Stage();

    // Disable copy to avoid double-free of raw shape pointers
    Stage(const Stage&) = delete;
    Stage& operator=(const Stage&) = delete;

    // Allow move (default)
    Stage(Stage&&) = default;
    Stage& operator=(Stage&&) = default;

    int getRow() const;
    int getColumn() const;
    Player& getPlayer();

    // Attempt to move the entity in the specified direction
    // Returns true if the move was successful, false otherwise
    bool moveEntitySuccessful(Entity& entity, std::string direction);

    void draw(sf::RenderWindow& window);
    void print() const;
};