#pragma once
#include <SFML/Graphics.hpp>
#include <iostream>

class Entity {
protected:
    float posX;
    float posY;
    sf::Sprite sprite;

public:
    Entity(float x, float y, const sf::Texture& texture) : posX(x), posY(y), sprite(texture) {
        sprite.setPosition({posX, posY});
    }
    virtual ~Entity() = default;

    virtual void move(float deltaTime) = 0;
    virtual void draw(sf::RenderWindow& window) = 0;
};

class Character : public Entity {
    
};

class Monster : public Entity {

};

class Projectile : public Entity {

};