#include <SFML/Graphics.hpp>
#include "Constants.hpp"
#include "Shape.hpp"
#include <vector>
#include <string>

RoundedRectangle::RoundedRectangle(float center_x, float center_y,
        float width, float height,
        float radius, float shadow_offset,
        const std::string& text_string, int text_size,
        const sf::Font& font,
        sf::Color rectangle_color, 
        sf::Color shadow_color,
        sf::Color text_color) : text(font, text_string, text_size) {
    // Initialize member variables
    rectangle_center_x = center_x;
    rectangle_center_y = center_y;
    rectangle_width = width;
    rectangle_height = height;
    circle_radius = radius;
    this->shadow_offset = shadow_offset;
    this->text_string = text_string;
    this->text_size = text_size;
    this->rectangle_color = rectangle_color;
    this->shadow_color = shadow_color;
    this->text_color = text_color;

    // Main part
    mainRectangleH = sf::RectangleShape({rectangle_width + 2 * circle_radius, rectangle_height});
    mainRectangleH.setOrigin({(rectangle_width + 2 * circle_radius) / 2.f, rectangle_height / 2.f});
    mainRectangleH.setPosition({rectangle_center_x, rectangle_center_y});
    mainRectangleH.setFillColor(rectangle_color);
    
    mainRectangleV = sf::RectangleShape({rectangle_width, rectangle_height + 2 * circle_radius});
    mainRectangleV.setOrigin({rectangle_width / 2.f, (rectangle_height + 2 * circle_radius) / 2.f});
    mainRectangleV.setPosition({rectangle_center_x, rectangle_center_y});
    mainRectangleV.setFillColor(rectangle_color);

    for(int i = 0; i < 4; ++i) {
        mainCorners.emplace_back(circle_radius);
        mainCorners.back().setOrigin({circle_radius, circle_radius});
        mainCorners.back().setFillColor(rectangle_color);
    }
    mainCorners[0].setPosition({rectangle_center_x - rectangle_width / 2.f, rectangle_center_y - rectangle_height / 2.f}); // UL
    mainCorners[1].setPosition({rectangle_center_x + rectangle_width / 2.f, rectangle_center_y - rectangle_height / 2.f}); // UR
    mainCorners[2].setPosition({rectangle_center_x - rectangle_width / 2.f, rectangle_center_y + rectangle_height / 2.f}); // LL
    mainCorners[3].setPosition({rectangle_center_x + rectangle_width / 2.f, rectangle_center_y + rectangle_height / 2.f}); // LR

    // Shadow part
    shadowRectangleH = sf::RectangleShape({rectangle_width + 2 * circle_radius, rectangle_height});
    shadowRectangleH.setOrigin({(rectangle_width + 2 * circle_radius) / 2.f, rectangle_height / 2.f});
    shadowRectangleH.setPosition({rectangle_center_x + shadow_offset, rectangle_center_y + shadow_offset});
    shadowRectangleH.setFillColor(shadow_color);

    shadowRectangleV = sf::RectangleShape({rectangle_width, rectangle_height + 2 * circle_radius});
    shadowRectangleV.setOrigin({rectangle_width / 2.f, (rectangle_height + 2 * circle_radius) / 2.f});
    shadowRectangleV.setPosition({rectangle_center_x + shadow_offset, rectangle_center_y + shadow_offset});
    shadowRectangleV.setFillColor(shadow_color);

    for(int i = 0; i < 4; i++) {
        shadowCorners.emplace_back(circle_radius);
        shadowCorners.back().setOrigin({circle_radius, circle_radius});
        shadowCorners.back().setFillColor(shadow_color);
    }
    shadowCorners[0].setPosition({rectangle_center_x - rectangle_width / 2.f + shadow_offset, rectangle_center_y - rectangle_height / 2.f + shadow_offset}); // UL
    shadowCorners[1].setPosition({rectangle_center_x + rectangle_width / 2.f + shadow_offset, rectangle_center_y - rectangle_height / 2.f + shadow_offset}); // UR
    shadowCorners[2].setPosition({rectangle_center_x - rectangle_width / 2.f + shadow_offset, rectangle_center_y + rectangle_height / 2.f + shadow_offset}); // LL
    shadowCorners[3].setPosition({rectangle_center_x + rectangle_width / 2.f + shadow_offset, rectangle_center_y + rectangle_height / 2.f + shadow_offset}); // LR

    // Click detection rectangle
    clickRectangle = sf::RectangleShape({rectangle_width + 2 * circle_radius, rectangle_height + 2 * circle_radius});
    clickRectangle.setOrigin({rectangle_width / 2.f, rectangle_height / 2.f});
    // Locate to top-left corner
    clickRectangle.setPosition({rectangle_center_x - circle_radius, rectangle_center_y - circle_radius});
    clickRectangle.setFillColor(sf::Color::Transparent); // Invisible
}

void RoundedRectangle::draw(sf::RenderWindow& window) {

    // Draw click detection rectangle (invisible)
    window.draw(clickRectangle);
    // Draw shadow first
    window.draw(shadowRectangleH);
    window.draw(shadowRectangleV);
    for(const auto& corner : shadowCorners) {
        window.draw(corner);
    }

    // Draw main rounded rectangle
    window.draw(mainRectangleH);
    window.draw(mainRectangleV);
    for(const auto& corner : mainCorners) {
        window.draw(corner);
    }

    // Draw text
    text.setOrigin({text.getLocalBounds().size.x / 2.f, text.getLocalBounds().size.y / 2.f});
    text.setPosition({rectangle_center_x, rectangle_center_y - text.getLocalBounds().position.y});
    text.setFillColor(text_color);
    window.draw(text);
}

bool RoundedRectangle::isClicked(sf::Vector2f mousePos) {
    return clickRectangle.getGlobalBounds().contains(mousePos);
}