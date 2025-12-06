#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>

float clamp(float value, float min, float max);
void handleDrag(const sf::RenderWindow& window, sf::View& view, sf::Vector2i& lastMousePos);
void setBackground(sf::Sprite& backgroundSprite, const sf::Texture& backgroundTexture);
std::string processPattern(const std::string& pattern);
void resizeTileTexture(sf::Sprite& sprite, int tile_size);
std::string trim(const std::string& str);
void cyclePattern(std::string& pattern);