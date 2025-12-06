#pragma once
#include <SFML/Graphics.hpp>
#include "Constants.hpp"
#include <vector>
#include <string>

class Object {
protected:
    sf::Sprite sprite;

public:
    // Position in tile coordinates
    sf::Vector2i posTile;
    // Position in window coordinates
    sf::Vector2f posWindow;

    Object(const sf::Texture& texture, sf::Vector2i posTile, sf::Vector2f posWindow, int tile_size);
    virtual ~Object() = default;

    sf::Sprite& getSprite();

    bool isValidMove(std::vector<std::vector<char>>& tileMap, sf::Vector2i newPosTile);
    virtual void update(std::vector<std::vector<char>>& tileMap, int tile_size) = 0;
    void draw(sf::RenderWindow& window, int tile_size);
};



// ========== Player Class =============
class Player : public Object {
private:
    void update(std::vector<std::vector<char>>& tileMap, int tile_size) override {}

public:
    Player(sf::Vector2i posTile, sf::Vector2f posWindow, int tile_size);
    bool isValidMove(std::vector<std::vector<char>>& tileMap, const Action& action);
    void update(std::vector<std::vector<char>>& tileMap, int tile_size, const Action& action);
};



// ========== Monster Class =============
class Monster : public Object {
public:
    Monster(const sf::Texture& texture, sf::Vector2i posTile, sf::Vector2f posWindow, int tile_size) : 
        Object(texture, posTile, posWindow, tile_size) {};
    bool isValidMove(std::vector<std::vector<char>>& tileMap, char actionChar);
    virtual void update(std::vector<std::vector<char>>& tileMap, int tile_size) override = 0;
};

class TraceMonster : public Monster {
public:
    TraceMonster(sf::Vector2i posTile, sf::Vector2f posWindow, int tile_size);
    void update(std::vector<std::vector<char>>& tileMap, int tile_size) override;
};

class GuardMonster : public Monster {
private:
    std::string behaviorPattern;

public:
    GuardMonster(sf::Vector2i posTile, sf::Vector2f posWindow, int tile_size, const std::string& pattern);
    void update(std::vector<std::vector<char>>& tileMap, int tile_size) override;
    std::string& getBehaviorPattern();
};



// ========== Dispensor Class =============
class Dispensor : public Object {
public:
    void update(std::vector<std::vector<char>>& tileMap, int tile_size) override;
};

class Projectile : public Object {
public:
    void update(std::vector<std::vector<char>>& tileMap, int tile_size) override;
};



// ========== Trap Class =============
class Trap : public Object {
public:
    void update(std::vector<std::vector<char>>& tileMap, int tile_size) override;
};