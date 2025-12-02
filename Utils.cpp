#include <SFML/Graphics.hpp>
#include "Utils.hpp"
#include "Constants.hpp"
#include "Logger.hpp"
#include <vector>
#include <string>

void createTiles(std::vector<sf::Shape*>& shapes, int grid_x, int grid_y, int tile_x, int tile_y) {
    int start_x = (WORLD_WIDTH - (grid_x * tile_x)) / 2;
    int start_y = (WORLD_HEIGHT - (grid_y * tile_y)) / 2;
    for (int i = 0; i < grid_y; i++) {
        for (int j = 0; j < grid_x; j++) {
            // Use new to allocate on heap to prevent going out of scope
            sf::RectangleShape* tile = new sf::RectangleShape(sf::Vector2f(tile_x, tile_y));
            
            // 計算當前方塊在遊戲世界中的座標
            float x = start_x + j * tile_x;
            float y = start_y + i * tile_y;
            
            tile->setPosition({x, y});

            // 設置方塊外觀
            tile->setOutlineThickness(1.0f);
            tile->setOutlineColor(sf::Color(100, 100, 100));
            
            // 棋盤格顏色
            if((i + j) % 2 == 0) {
                tile->setFillColor(sf::Color(60, 60, 60)); 
            } else {
                tile->setFillColor(sf::Color(40, 40, 40)); 
            }
            
            shapes.push_back(tile);
            Logger::log_debug("Created tile at (" + std::to_string(x) + ", " + std::to_string(y) + ").");
        }
    }
}

float clamp(float value, float min, float max) {
    return std::max(min, std::min(max, value));
}

void handleDrag(const sf::RenderWindow& window, sf::View& view, sf::Vector2i& lastMousePos) {
        // Get the current mouse position
        sf::Vector2i currentMousePos = sf::Mouse::getPosition(window);

        // Compute the mouse movement delta in pixels
        sf::Vector2i pixelDelta = currentMousePos - lastMousePos;

        // Key step: convert pixel delta to world coordinate delta
        // Since the View may be zoomed, we need to use mapPixelToCoords for accurate conversion.

        // Point A in pixel coordinates (0, 0)
        sf::Vector2f worldCoordA = window.mapPixelToCoords({0, 0}, view);
        
        // Point B in pixel coordinates (pixelDelta.x, pixelDelta.y)
        sf::Vector2f worldCoordB = window.mapPixelToCoords(pixelDelta, view);

        // Movement in world coordinates = (B - A)
        // Since we need the opposite direction movement (dragging screen right moves View center left)
        sf::Vector2f worldDelta = worldCoordA - worldCoordB;

        // Restrict View within world bounds
        sf::Vector2f currentCenter = view.getCenter();
        float targetX = currentCenter.x + worldDelta.x;
        float targetY = currentCenter.y + worldDelta.y;
        
        // Ensure the View center stays within [0, WORLD_WIDTH] and [0, WORLD_HEIGHT]
        
        // Restrict X to [0, WORLD_WIDTH], Y to [0, WORLD_HEIGHT]
        float newCenterX = clamp(targetX, 0.0f, WORLD_WIDTH);
        float newCenterY = clamp(targetY, 0.0f, WORLD_HEIGHT);
        view.setCenter({newCenterX, newCenterY});

        lastMousePos = currentMousePos;
        
        Logger::log_debug("View moved to (" + std::to_string(view.getCenter().x) + ", " + std::to_string(view.getCenter().y) + ")");
}