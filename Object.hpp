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

    Object(const sf::Texture& texture, sf::Vector2i posTile, sf::Vector2f posWindow, int tileSize);
    virtual ~Object() = default;

    sf::Sprite& getSprite();

    bool isValidAction(std::vector<std::vector<char>>& tileMap, sf::Vector2i newPosTile);
    virtual void update(std::vector<std::vector<char>>& tileMap, int tileSize) = 0;
    void draw(sf::RenderWindow& window, int tileSize);
};



// ========== Player Class =============
class Player : public Object {
private:
    void update(std::vector<std::vector<char>>& tileMap, int tileSize) override {}

public:
    Player(sf::Vector2i posTile, sf::Vector2f posWindow, int tileSize);
    bool isValidMove(std::vector<std::vector<char>>& tileMap, const Action& action);
    void update(std::vector<std::vector<char>>& tileMap, int tileSize, const Action& action);
};



// ========== Wall and Goal Class =============
class Wall : public Object {
private:
    void update(std::vector<std::vector<char>>& tileMap, int tileSize) override {}

public:
    Wall(sf::Vector2i posTile, sf::Vector2f posWindow, int tileSize);
};

class Goal : public Object {
private:
    void update(std::vector<std::vector<char>>& tileMap, int tileSize) override {}
    
public:
    Goal(sf::Vector2i posTile, sf::Vector2f posWindow, int tileSize);
};



// ========== Monster Class =============
class Monster : public Object {
public:
    Monster(const sf::Texture& texture, sf::Vector2i posTile, sf::Vector2f posWindow, int tileSize) : 
        Object(texture, posTile, posWindow, tileSize) {};
    bool isValidMove(std::vector<std::vector<char>>& tileMap, char actionChar);
    virtual void update(std::vector<std::vector<char>>& tileMap, int tileSize) override = 0;
};

class TraceMonster : public Monster {
private:
    void update(std::vector<std::vector<char>>& tileMap, int tileSize) override {}
public:
    TraceMonster(sf::Vector2i posTile, sf::Vector2f posWindow, int tileSize);
    void update(std::vector<std::vector<char>>& tileMap, int tileSize, const sf::Vector2i& playerPosTile);
};

class GuardMonster : public Monster {
private:
    std::string behaviorPattern;

public:
    GuardMonster(sf::Vector2i posTile, sf::Vector2f posWindow, int tileSize, const std::string& pattern);
    void update(std::vector<std::vector<char>>& tileMap, int tileSize) override;
    std::string& getBehaviorPattern();
};



// ========== Dispenser Class =============
class Dispenser : public Object {
private:
    std::string behaviorPattern;
    void update(std::vector<std::vector<char>>& tileMap, int tileSize) override {}

public:
    Dispenser(sf::Vector2i posTile, sf::Vector2f posWindow, int tileSize, const std::string& pattern);
    bool isSpawnable(std::vector<std::vector<char>>& tileMap, char actionChar);
    void update(std::vector<std::vector<char>>& tileMap, int tileSize, std::vector<std::unique_ptr<Object>>& bufferObjects);
    std::string& getBehaviorPattern();
};

class Projectile : public Object {
protected:
    sf::Vector2i originalPosTile;
    char direction;

public:
    Projectile(const sf::Texture& texture, sf::Vector2i posTile, sf::Vector2f posWindow, int tileSize, char direction);
    virtual void update(std::vector<std::vector<char>>& tileMap, int tileSize) override = 0;

    sf::Vector2i getOriginalPosTile() const;
};

class Arrow : public Projectile {
public:
    Arrow(sf::Vector2i posTile, sf::Vector2f posWindow, int tileSize, char direction);
    void update(std::vector<std::vector<char>>& tileMap, int tileSize) override;
};



// ========== Trap Class =============
class Trap : public Object {
public:
    Trap(sf::Vector2i posTile, sf::Vector2f posWindow, int tileSize);
    void update(std::vector<std::vector<char>>& tileMap, int tileSize) override;
};