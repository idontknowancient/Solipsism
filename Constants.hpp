#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <string>

enum class GameState {
    TitleScreen,
    Playing,
};

enum class Action {
    MoveUp,
    MoveDown,
    MoveLeft,
    MoveRight,
    Attack,
    None,
};

// Inline ensures single definition across translation units
// Global settings
inline const bool DEBUG_MODE = true;
inline const int WORLD_WIDTH = 1920;
inline const int WORLD_HEIGHT = 1080;
inline const int FRAME_RATE = 60;

// Resource files
inline const std::string GAME_TITLE = "Solipsism";
inline const std::string ICON_FILE = "assets/icon.png";
inline const std::string BGM_FILE = "assets/sølips.mp3";
inline const std::string TITLE_IMAGE_FILE = "assets/title_image.png";
inline const std::string BACKGROUND_IMAGE_FILE = "assets/background_image.jpg";
inline const std::string BUTTON_TEXTURE_FILE = "assets/start_button.png";
inline const std::string PLAYER_TEXTURE_FILE = "assets/player.jpg";
inline const std::string GUARD_MONSTER_TEXTURE_FILE = "assets/guard_monster.png";
inline const std::string TRACE_MONSTER_TEXTURE_FILE = "assets/trace_monster.png";
inline const std::string DISPENSER_TEXTURE_FILE = "assets/dispenser.png";
inline const std::string ARROW_TEXTURE_FILE = "assets/arrow.png";
inline const std::string BUTTON_FONT_FILE = "assets/Conthrax.otf";
inline const std::string STAGE_FILE = "stages.txt";

inline const float BGM_VOLUME = 10.f;
inline const float ZOOM_RATE = 0.1f; // Rate of zooming in/out per mouse wheel scroll

// Colors
inline const sf::Color BACKGROUND_TRANSLUCENT = sf::Color(255, 255, 255, 50);
inline const sf::Color TILE_COLOR_PLAYER = sf::Color(30, 80, 162);
inline const sf::Color TILE_COLOR_GOAL = sf::Color(147, 202, 118);
inline const sf::Color TILE_COLOR_NORMAL = sf::Color(114, 113, 113);
inline const sf::Color TILE_COLOR_WALL = sf::Color(31, 49, 52);
inline const sf::Color TILE_COLOR_DISPENSER = sf::Color(240, 131, 0);
inline const sf::Color TILE_COLOR_TRACE_MONSTER = sf::Color(217, 51, 63);
inline const sf::Color TILE_COLOR_GUARD_MONSTER = sf::Color(239, 171, 147);

// Object symbols
inline const char SYMBOL_PLAYER = 'P';
inline const char SYMBOL_GOAL = 'G';
inline const char SYMBOL_WALL = 'X';
inline const char SYMBOL_OPEN_SPACE = '-';
inline const char SYMBOL_DISPENSER = 'D';
inline const char SYMBOL_TRACE_MONSTER = 'M';
inline const char SYMBOL_GUARD_MONSTER = 'm';

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
    static sf::Image icon;
    static sf::Music music;
    static sf::Texture titleTexture;
    static sf::Texture titleBackgroundTexture;
    static sf::Font buttonFont;
    static sf::Texture playerTexture;
    static sf::Texture guardMonsterTexture;
    static sf::Texture traceMonsterTexture;
    static sf::Texture dispenserTexture;
    static sf::Texture arrowTexture;

public:
    Resource() = delete;
    ~Resource() = delete;

    static void init();
    static const sf::Image& getIcon();
    static sf::Music& getMusic();
    static const sf::Texture& getTitleTexture();
    static const sf::Texture& getTitleBackgroundTexture();
    static const sf::Font& getButtonFont();
    static const sf::Texture& getPlayerTexture();
    static const sf::Texture& getGuardMonsterTexture();
    static const sf::Texture& getTraceMonsterTexture();
    static const sf::Texture& getDispenserTexture();
    static const sf::Texture& getArrowTexture();
};