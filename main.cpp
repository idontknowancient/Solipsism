#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include "Constants.hpp"
#include "Logger.hpp"
#include "Utils.hpp"
#include "Shape.hpp"
#include <iostream>
#include <fstream>
#include <vector>
#include <string>

int main() {

    // Initialize logger
    Logger::init("debug_log.txt");
    Logger::log("Game started.");



    // Create the main window
    sf::RenderWindow window(sf::VideoMode({WORLD_WIDTH, WORLD_HEIGHT}), GAME_TITLE);
    // Set a framerate limit (not depending on device refresh rate)
    // setFramerateLimit and setVerticalSyncEnabled should not be used together
    window.setFramerateLimit(FRAME_RATE);
    // window.setVerticalSyncEnabled(true);



    // View setup (used to control what is shown in the window)
    sf::View view;
    view.setCenter({WORLD_WIDTH / 2.f, WORLD_HEIGHT / 2.f});
    view.setSize({WORLD_WIDTH, WORLD_HEIGHT});
    window.setView(view);



    // BGM setup
    sf::Music music;
    if(music.openFromFile(BGM_FILE)) {
        music.play();
        music.setVolume(BGM_VOLUME);
        music.setLooping(true);
    } else {
        Logger::log("Failed to load BGM file: " + BGM_FILE);
    }



    // Gamestate
    GameState gameState = GameState::TitleScreen;

    // Title image assets
    sf::Texture titleTexture;
    if(!titleTexture.loadFromFile(TITLE_IMAGE_FILE)) {
        Logger::log("Failed to load title image: " + TITLE_IMAGE_FILE);
    }
    sf::Sprite titleSprite(titleTexture);
    titleSprite.setPosition({(WORLD_WIDTH - titleTexture.getSize().x) / 2.f, -50.f}); // 水平置中，Y 軸 150 單位下移
    titleSprite.setColor(sf::Color(255, 255, 255, 180)); // 微透明效果
    
    // Background image assets
    sf::Texture backgroundTexture;
    if(!backgroundTexture.loadFromFile(BACKGROUND_IMAGE_FILE)) {
        Logger::log("Failed to load title image: " + BACKGROUND_IMAGE_FILE);
    }
    sf::Sprite backgroundSprite(backgroundTexture);
    // 計算圖片原始尺寸
    sf::Vector2u backgroundSize = backgroundTexture.getSize();
    
    // 獲取當前 View 尺寸 (使用 WORLD_WIDTH/HEIGHT 作為初始 View 尺寸，用於計算)
    float viewWidth = WORLD_WIDTH;
    float viewHeight = WORLD_HEIGHT; 
    
    // 1. 計算縮放比例
    // 必須使用較大的比例 (Cover 模式)
    float scaleX = viewWidth / backgroundSize.x;
    float scaleY = viewHeight / backgroundSize.y;
    float finalScale = std::max(scaleX, scaleY); // 選擇最大的比例以確保完全覆蓋

    // 2. 應用縮放
    backgroundSprite.setScale({finalScale, finalScale});

    // 3. 設置原點到中心，並將圖片中心對齊 View 中心
    // 這樣可以確保圖片在 View 中居中，多餘的部分被裁剪
    sf::Vector2f spriteCenter = {backgroundSprite.getGlobalBounds().size.x / 2.f, backgroundSprite.getGlobalBounds().size.y / 2.f};
    backgroundSprite.setOrigin({spriteCenter.x / finalScale, spriteCenter.y / finalScale}); // 原點仍需基於未縮放尺寸
    
    // 設置位置 (對齊 View 中心)
    backgroundSprite.setPosition({viewWidth / 2.f, viewHeight / 2.f});
    backgroundSprite.setColor(sf::Color(255, 255, 255, 50)); // 半透明效果

    // Font
    sf::Font font;
    if(!font.openFromFile(FONT_FILE.data())) {
        Logger::log("Failed to load font file: " + std::string(FONT_FILE.data()));
    }

    // Start button
    RoundedRectangle startButton(
        BUTTON_CENTER_X, BUTTON_CENTER_Y,
        BUTTON_RECTANGLE_WIDTH, BUTTON_RECTANGLE_HEIGHT,
        BUTTON_CIRCLE_RADIUS, BUTTON_SHADOW_OFFSET,
        "START", 30, font
    );



    // Store all shapes drawn
    // Later binding
    std::vector<sf::Shape*> shapes;



    // Used for dragging view
    bool isDragging = false;
    sf::Vector2i lastMousePos;


    // Start the game loop
    while(window.isOpen()) {

        // I: Process events
        while(const std::optional event = window.pollEvent()) {
            if(event->is<sf::Event::Closed>()) {
                music.stop();
                window.close();
                Logger::log("Window closed by user.");
            } 
            
            else if (const auto* resized = event->getIf<sf::Event::Resized>()) {
                // Update the view to the new size of the window
                sf::FloatRect visibleArea({0.f, 0.f}, sf::Vector2f(resized->size));
                window.setView(sf::View(visibleArea));
                Logger::log("Window resized to " + std::to_string(resized->size.x) + "x" + std::to_string(resized->size.y) + ".");
            } 
            
            else if(const auto* mousePressed = event->getIf<sf::Event::MouseButtonPressed>()) {
                if(mousePressed->button == sf::Mouse::Button::Left) {

                    if(gameState == GameState::TitleScreen) {
                        // Map pixel to world coordinates
                        sf::Vector2f worldPos = window.mapPixelToCoords(mousePressed->position);

                        // Check if start button is pressed
                        if(startButton.isClicked(worldPos)) {
                            gameState = GameState::Playing;
                            Logger::log("Start Game button pressed. Entering Playing state.");

                            createTiles(shapes, 8, 8, 100, 100);
                        }
                    }

                    else if(gameState == GameState::Playing) {
                        isDragging = true;
                    }

                    lastMousePos = mousePressed->position;
                    Logger::log("Clicked left button at (" + std::to_string(lastMousePos.x) + ", " + std::to_string(lastMousePos.y) + ").");
                }
            } 
            
            else if(const auto* mouseReleased = event->getIf<sf::Event::MouseButtonReleased>()) {
                if(mouseReleased->button == sf::Mouse::Button::Left) {
                    isDragging = false;
                }
            } 
            
            else if(const auto* mouseMoved = event->getIf<sf::Event::MouseMoved>()) {
                
            } 
            
            else if(const auto* mouseWheel = event->getIf<sf::Event::MouseWheelScrolled>()) {
                // Check if vertical scroll
                if(mouseWheel->wheel == sf::Mouse::Wheel::Vertical) {
                    //  > 0 meaning zoom in and restrict max/min zoom levels
                    if(mouseWheel->delta > 0 && view.getSize().x > 500.f && view.getSize().y > 500.f) {
                        // 1.0 - 0.1 = 0.9
                        view.zoom(1.0f - ZOOM_RATE);
                        Logger::log("Zoomed in.");
                        Logger::log_debug("View size: (" + std::to_string(view.getSize().x) + ", " + std::to_string(view.getSize().y) + ").");
                    } else if(mouseWheel->delta < 0 && view.getSize().x < WORLD_WIDTH * 2.f && view.getSize().y < WORLD_HEIGHT * 2.f) {
                        // 1.0 + 0.1 = 1.1
                        view.zoom(1.0f + ZOOM_RATE);
                        Logger::log("Zoomed out.");
                        Logger::log_debug("View size: (" + std::to_string(view.getSize().x) + ", " + std::to_string(view.getSize().y) + ").");
                    }
                }
            }
        }



        // II: Handle
        if(isDragging && gameState == GameState::Playing) handleDrag(window, view, lastMousePos);



        // III: Update
        // Clear screen
        window.clear();

        window.setView(view);

        // Drawing logic
        if(gameState == GameState::TitleScreen) {
            // Use default view for UI
            window.setView(window.getDefaultView());
            window.draw(backgroundSprite);
            window.draw(titleSprite);
            
            // Draw start button
            startButton.draw(window);
        } else if(gameState == GameState::Playing) {
            window.setView(view);

            // Draw all shapes
            for(const auto& shape : shapes) {
                window.draw(*shape);
            }
        }

        // Update the window
        window.display();
    }



    // Cleanup and exit
    Logger::log("Game exited.");
    Logger::shutdown();

    // Free allocated memory
    for(auto& shape : shapes) {
        delete shape;
    }

    return 0;
}