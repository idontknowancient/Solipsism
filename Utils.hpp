#pragma once
#include <SFML/Graphics.hpp>
#include <vector>

void createTiles(std::vector<sf::Shape*>& shapes, int grid_x, int grid_y, int tile_x, int tile_y);
float clamp(float value, float min, float max);
void handleDrag(const sf::RenderWindow& window, sf::View& view, sf::Vector2i& lastMousePos);