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

// Process pattern: expand letters with numbers (e.g. U2D2L4 -> UUDDLLLL)
std::string processPattern(const std::string& pattern) {
    std::string expandedPattern;
    for(int i = 0; i < pattern.size(); i++) {
        char ch = pattern[i];
        if(std::isalpha(ch)) {
            // Check if followed by a digit
            if(i + 1 < pattern.size() && std::isdigit(pattern[i + 1])) {
                int repeatCount = pattern[i + 1] - '0';
                // Handle multi-digit numbers
                int j = i + 2;
                while(j < pattern.size() && std::isdigit(pattern[j])) {
                    repeatCount = repeatCount * 10 + (pattern[j] - '0');
                    j++;
                }
                // Skip the digits
                i = j - 1;
                // Repeat the character
                for(int k = 0; k < repeatCount; k++) {
                    expandedPattern += ch;
                }
            } else {
                // No number after letter, add once
                expandedPattern += ch;
            }
        } else if(!std::isdigit(ch)) {
            // Keep non-alphanumeric characters (like semicolons)
            expandedPattern += ch;
        }
    }
    
    return expandedPattern;
}

void resizeTileTexture(sf::Sprite& sprite, int tile_size) {
    sf::Vector2u textureSize = sprite.getTexture().getSize();
    float scaleX = static_cast<float>(tile_size) / textureSize.x;
    float scaleY = static_cast<float>(tile_size) / textureSize.y;
    sprite.setScale({scaleX, scaleY});
}

// Remove leading and trailing spaces from a string
std::string trim(const std::string& str) {
    size_t first = str.find_first_not_of(' ');
    if (std::string::npos == first) return str;
    size_t last = str.find_last_not_of(' ');
    return str.substr(first, (last - first + 1));
}

// Cycle pattern string: move first character to the end
void cyclePattern(std::string& pattern) {
    if(pattern.empty()) {
        Logger::log_debug("Warning: Attempted to cycle an empty pattern string.");
        return;
    }

    // Get the first character
    char firstChar = pattern[0];

    // Remove the first character from the beginning of the string
    // The parameters of erase() are the starting iterator and ending iterator.
    // pattern.begin() points to the first character, pattern.begin() + 1 points to the second character.
    pattern.erase(pattern.begin()); 

    // Append the character to the end of the string
    pattern += firstChar;

    Logger::log_debug("Pattern cycled: " + pattern);
}