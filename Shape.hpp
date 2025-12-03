#pragma once
#include <SFML/Graphics.hpp>
#include "Constants.hpp"
#include <vector>
#include <string>

class RoundedRectangle {
protected:
    float rectangle_center_x;
    float rectangle_center_y;
    float rectangle_width;
    float rectangle_height;
    float circle_radius;
    float shadow_offset;
    std::string text_string;
    int text_size;

    sf::Color rectangle_color;
    sf::Color shadow_color;
    sf::Color text_color;
    sf::Font font;
    sf::Text text;

    // Use two rectangles and four circles to form a rounded rectangle
    sf::RectangleShape mainRectangleH;
    sf::RectangleShape mainRectangleV;
    std::vector<sf::CircleShape> mainCorners;

    sf::RectangleShape shadowRectangleH;
    sf::RectangleShape shadowRectangleV;
    std::vector<sf::CircleShape> shadowCorners;

    // For click detection
    sf::RectangleShape clickRectangle;
    
public:
    RoundedRectangle(float center_x, float center_y,
                     float width, float height,
                     float radius, float shadow_offset,
                     const std::string& text_string, int text_size,
                     const sf::Font& font,
                     sf::Color rectangle_color = BUTTON_RECTANGLE_COLOR, 
                     sf::Color shadow_color = BUTTON_SHADOW_COLOR,
                     sf::Color text_color = BUTTON_TEXT_COLOR);

    void draw(sf::RenderWindow& window);
    bool isClicked(sf::Vector2f mousePos);
};