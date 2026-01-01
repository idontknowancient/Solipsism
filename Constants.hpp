#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <string>

enum class GameState {
    TitleScreen,
    StageSelect,
    Playing,
    GameOver,
    StageClear,
};

enum class Action {
    MoveUp,
    MoveDown,
    MoveLeft,
    MoveRight,
    Attack,
    None,
};

// Configuration class to load settings from file
class Config {
public:
    // Global settings
    static bool DEBUG_MODE;
    static int WORLD_WIDTH;
    static int WORLD_HEIGHT;
    static int FRAME_RATE;
    static float BGM_VOLUME;
    static float ZOOM_RATE;
    
    // Load configuration from file
    static void init(const std::string& configFile = "config.txt");
    
private:
    Config() = delete;
    ~Config() = delete;
};

// Resource files
inline const std::string GAME_TITLE = "Solipsism";
inline const std::string ICON_FILE = "assets/icon.png";
inline const std::string BGM_FILE = "assets/sølips.mp3";
inline const std::string TITLE_IMAGE_FILE = "assets/title_image.png";
inline const std::string BACKGROUND_TITLE_FILE = "assets/background_title.png";
inline const std::string BACKGROUND_STAGE_FILE = "assets/background_stage.jpg";
inline const std::string STAGE_CLEAR_FILE = "assets/stage_clear.png";
inline const std::string BUTTON_TEXTURE_FILE = "assets/start_button.png";
inline const std::string PLAYER_TEXTURE_FILE = "assets/player.png";
inline const std::string OPEN_SPACE_1_TEXTURE_FILE = "assets/open_space.jpg";
inline const std::string OPEN_SPACE_2_TEXTURE_FILE = "assets/open_space_2.jpg";
inline const std::string OPEN_SPACE_3_TEXTURE_FILE = "assets/open_space_3.jpg";
inline const std::string OPEN_SPACE_4_TEXTURE_FILE = "assets/open_space_4.jpg";
inline const std::string WALL_TEXTURE_FILE = "assets/wall.jpg";
inline const std::string GOAL_TEXTURE_FILE = "assets/goal.jpg";
inline const std::string GUARD_MONSTER_TEXTURE_FILE = "assets/guard_monster.png";
inline const std::string TRACE_MONSTER_TEXTURE_FILE = "assets/trace_monster.png";
inline const std::string DISPENSER_TEXTURE_FILE = "assets/dispenser.png";
inline const std::string ARROW_TEXTURE_FILE = "assets/arrow.png";
inline const std::string TRAP_TEXTURE_FILE = "assets/trap.jpg";
inline const std::string BUTTON_FONT_FILE = "assets/Conthrax.otf";
inline const std::string STAGE_FILE = "stages.txt";

// BGM_VOLUME and ZOOM_RATE moved to Config class

// Colors
inline const sf::Color BACKGROUND_TRANSLUCENT = sf::Color(255, 255, 255, 150);
inline const sf::Color BACKGROUND_TRANSLUCENT_STRONGER = sf::Color(255, 255, 255, 50);
inline const sf::Color STAGE_CLEAR_TRANSLUCENT = sf::Color(40, 40, 40, 150);
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
inline const char SYMBOL_ARROW = 'A';

// Direction symbols
inline const char SYMBOL_UP = 'U';
inline const char SYMBOL_DOWN = 'D';
inline const char SYMBOL_LEFT = 'L';
inline const char SYMBOL_RIGHT = 'R';

// Default Button Settings
inline const float BUTTON_WIDTH = 240.0f;
inline const float BUTTON_HEIGHT = 80.0f;
inline const float BUTTON_CENTER_X = WORLD_WIDTH / 2.f;
inline const float BUTTON_CENTER_Y = WORLD_HEIGHT / 2.f + 100.f; // 100 pixels below center
inline const float BUTTON_CIRCLE_RADIUS = 30.f;
inline const float BUTTON_RECTANGLE_WIDTH = BUTTON_WIDTH - 2 * BUTTON_CIRCLE_RADIUS;
inline const float BUTTON_RECTANGLE_HEIGHT = BUTTON_HEIGHT - 2 * BUTTON_CIRCLE_RADIUS;
inline const float BUTTON_SHADOW_OFFSET = 4.f;
inline const sf::Color BUTTON_RECTANGLE_COLOR = sf::Color(40, 40, 40);
inline const sf::Color BUTTON_SHADOW_COLOR = sf::Color(0, 0, 0, 150);
inline const sf::Color BUTTON_TEXT_COLOR = sf::Color(200, 200, 200);

class Config {
private:
    static bool DEBUG_MODE;
    static int WORLD_WIDTH;
    static int WORLD_HEIGHT;
    static int FRAME_RATE;

public:
    Config() = delete;
    ~Config() = delete;

    static void init();

    static bool isDebugMode();
    static void setDebugMode(bool debug);

    static int getWorldWidth();
    static void setWorldWidth(int width);

    static int getWorldHeight();
    static void setWorldHeight(int height);

    static int getFrameRate();
    static void setFrameRate(int frameRate);
};

class Resource {
private:
    static sf::Image icon;
    static sf::Music music;
    static sf::Texture titleTexture;
    static sf::Texture backgroundTitleTexture;
    static sf::Texture backgroundStageTexture;
    static sf::Texture stageClearTexture;
    static sf::Font buttonFont;
    static sf::Texture playerTexture;
    static sf::Texture openSpaceTexture1;
    static sf::Texture openSpaceTexture2;
    static sf::Texture openSpaceTexture3;
    static sf::Texture openSpaceTexture4;
    static sf::Texture wallTexture;
    static sf::Texture goalTexture;
    static sf::Texture guardMonsterTexture;
    static sf::Texture traceMonsterTexture;
    static sf::Texture dispenserTexture;
    static sf::Texture arrowTexture;
    static sf::Texture trapTexture;

public:
    Resource() = delete;
    ~Resource() = delete;

    static void init();
    static const sf::Image& getIcon();
    static sf::Music& getMusic();
    static const sf::Texture& getTitleTexture();
    static const sf::Texture& getBackgroundTitleTexture();
    static const sf::Texture& getBackgroundStageTexture();
    static const sf::Texture& getStageClearTexture();
    static const sf::Font& getButtonFont();
    static const sf::Texture& getPlayerTexture();
    static const sf::Texture& getOpenSpaceTexture(int variant = 1);
    static const sf::Texture& getWallTexture();
    static const sf::Texture& getGoalTexture();
    static const sf::Texture& getGuardMonsterTexture();
    static const sf::Texture& getTraceMonsterTexture();
    static const sf::Texture& getDispenserTexture();
    static const sf::Texture& getArrowTexture();
    static const sf::Texture& getTrapTexture();
};