#pragma once
#include <SFML/Graphics.hpp>
#include <string>

enum class GameState {
    TitleScreen,
    Playing
};

// Inline ensures single definition across translation units
// Global settings
inline const bool DEBUG_MODE = true;
inline const int WORLD_WIDTH = 1920;
inline const int WORLD_HEIGHT = 1080;
inline const int FRAME_RATE = 60;

// Resource files
inline const std::string GAME_TITLE = "Solipsism";
inline const std::string BGM_FILE = "assets/sølips.mp3";
inline const std::string TITLE_IMAGE_FILE = "assets/title_image.png";
inline const std::string BACKGROUND_IMAGE_FILE = "assets/background_image.jpg";
inline const std::string BUTTON_TEXTURE_FILE = "assets/start_button.png";
inline const std::string PLAYER_TEXTURE_FILE = "assets/player.jpg";
inline const std::string FONT_FILE = "assets/Conthrax.otf";

inline const float BGM_VOLUME = 20.f;
inline const float ZOOM_RATE = 0.1f; // Rate of zooming in/out per mouse wheel scroll

// Default Button Settings
inline const float BUTTON_WIDTH = 200.0f;
inline const float BUTTON_HEIGHT = 80.0f;
inline const float BUTTON_CENTER_X = WORLD_WIDTH / 2.f;
inline const float BUTTON_CENTER_Y = WORLD_HEIGHT / 2.f + 100.f; // 100 pixels below center
inline const float BUTTON_CIRCLE_RADIUS = 30.f;
inline const float BUTTON_RECTANGLE_WIDTH = BUTTON_WIDTH - 2 * BUTTON_CIRCLE_RADIUS;
inline const float BUTTON_RECTANGLE_HEIGHT = BUTTON_HEIGHT - 2 * BUTTON_CIRCLE_RADIUS;
inline const float BUTTON_SHADOW_OFFSET = 4.f;
inline const sf::Color BUTTON_RECTANGLE_COLOR = sf::Color(40, 40, 40, 200);
inline const sf::Color BUTTON_SHADOW_COLOR = sf::Color(0, 0, 0, 150);
inline const sf::Color BUTTON_TEXT_COLOR = sf::Color(200, 200, 200);

class Resource {
private:
    static sf::Texture playerTexture;

public:
    Resource() = delete;
    ~Resource() = delete;

    static void init();
    static const sf::Texture& getPlayerTexture();
};