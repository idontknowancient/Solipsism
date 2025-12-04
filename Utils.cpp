#include <SFML/Graphics.hpp>
#include "Utils.hpp"
#include "Constants.hpp"
#include "Logger.hpp"
#include <vector>
#include <string>

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

void setBackground(sf::Sprite& backgroundSprite, const sf::Texture& backgroundTexture) {
    // Original size of the background texture
    sf::Vector2u backgroundSize = backgroundTexture.getSize();
    
    float viewWidth = WORLD_WIDTH;
    float viewHeight = WORLD_HEIGHT; 
    
    // 1. Calculate scaling factor
    // Must use the larger scale (Cover mode)
    float scaleX = viewWidth / backgroundSize.x;
    float scaleY = viewHeight / backgroundSize.y;
    float finalScale = std::max(scaleX, scaleY); // Choose the largest scale to ensure full coverage
    // 2. Apply scaling
    backgroundSprite.setScale({finalScale, finalScale});

    // 3. Set origin to center and align sprite center to View center
    // This ensures the sprite is centered in the View and excess parts are cropped
    sf::Vector2f spriteCenter = {backgroundSprite.getGlobalBounds().size.x / 2.f, backgroundSprite.getGlobalBounds().size.y / 2.f};
    backgroundSprite.setOrigin({spriteCenter.x / finalScale, spriteCenter.y / finalScale}); // Origin should still be based on unscaled size
    
    // Set position (align with View center)
    backgroundSprite.setPosition({viewWidth / 2.f, viewHeight / 2.f});
    backgroundSprite.setColor(BACKGROUND_TRANSLUCENT);
}