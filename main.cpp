#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include "Constants.hpp"
#include "Logger.hpp"
#include "Utils.hpp"
#include "Shape.hpp"
#include "Object.hpp"
#include "Stage.hpp"
#include <iostream>
#include <fstream>
#include <vector>
#include <string>

int main() {

    // Initialize logger and resources
    Logger::init("debug_log.txt");
    Logger::log("Game started.");
    Resource::init();



    // Create the main window
    sf::RenderWindow window(sf::VideoMode({WORLD_WIDTH, WORLD_HEIGHT}), GAME_TITLE);
    // Set icon (convert sf::Image to icon format)
    const sf::Image& iconImg = Resource::getIcon();
    if(iconImg.getSize().x > 0) {
        window.setIcon(iconImg);
    }
    // Set a framerate limit (not depending on device refresh rate)
    // setFramerateLimit and setVerticalSyncEnabled should not be used together
    window.setFramerateLimit(FRAME_RATE);
    // window.setVerticalSyncEnabled(true);



    // View setup (used to control what is shown in the window)
    sf::View view;
    view.setCenter({WORLD_WIDTH / 2.f, WORLD_HEIGHT / 2.f});
    view.setSize({WORLD_WIDTH, WORLD_HEIGHT});
    window.setView(view);



    // Title image
    sf::Sprite titleSprite(Resource::getTitleTexture());
    titleSprite.setPosition({(WORLD_WIDTH - Resource::getTitleTexture().getSize().x) / 2.f, -50.f}); // 水平置中，Y 軸 150 單位下移
    titleSprite.setColor(sf::Color(255, 255, 255, 180)); // 微透明效果
    
    // Background image
    sf::Sprite backgroundSprite(Resource::getBackgroundTitleTexture());
    setBackground(backgroundSprite, Resource::getBackgroundTitleTexture());

    // Start button
    RoundedRectangle startButton(
        BUTTON_CENTER_X, BUTTON_CENTER_Y,
        BUTTON_RECTANGLE_WIDTH, BUTTON_RECTANGLE_HEIGHT,
        BUTTON_CIRCLE_RADIUS, BUTTON_SHADOW_OFFSET,
        "START", 30, Resource::getButtonFont()
    );



    // Gamestate
    GameState gameState = GameState::TitleScreen;
    int stageNum = 0;
    int stageIndex = 1;



    // Store all stages
    std::vector<Stage> stages;
    Stage::createFromFile(stages);

    // Used for dragging view
    bool isDragging = false;
    sf::Vector2i lastMousePos;



    // Start the game loop
    while(window.isOpen()) {

        // I: Process events
        while(const std::optional event = window.pollEvent()) {
            if(event->is<sf::Event::Closed>()) {
                Resource::getMusic().stop();
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
                    Logger::log("Clicked left button at (" + std::to_string(mousePressed->position.x) + ", " 
                        + std::to_string(mousePressed->position.y) + ").");

                    if(gameState == GameState::TitleScreen) {
                        // Map pixel to world coordinates
                        sf::Vector2f worldPos = window.mapPixelToCoords(mousePressed->position);

                        // Check if start button is pressed
                        if(startButton.isClicked(worldPos)) {
                            gameState = GameState::Playing;
                            Logger::log("Start Game button pressed. Entering Playing state.");

                            // Create stages if not already created
                            if(stages.empty()) {
                                stageNum = static_cast<int>(stages.size());
                            }
                        }
                    }

                    else if(gameState == GameState::Playing) {
                        isDragging = true;
                    }

                    lastMousePos = mousePressed->position;
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
                    // handleScroll(view, mouseWheel);
                }
            }

            else if(const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {
                if(keyPressed->code == sf::Keyboard::Key::Escape) {
                    Logger::log("Escape key pressed.");

                    if(gameState == GameState::Playing) {
                        Logger::log("Returning to Title Screen.");
                        gameState = GameState::TitleScreen;
                    } else if(gameState == GameState::StageClear) {
                        stages.at(stageIndex - 1).reset();
                        Logger::log("Returning to Title Screen from Stage Clear.");
                        gameState = GameState::TitleScreen;
                    }
                }

                if(stages.empty()) continue;
                Stage& currentStage = stages.at(stageIndex - 1);

                // Press R to reset stage
                if(keyPressed->code == sf::Keyboard::Key::R) {
                    Logger::log("R key pressed.");

                    if(gameState == GameState::Playing) {
                        currentStage.reset();
                    }
                }

                if(keyPressed->code == sf::Keyboard::Key::W || keyPressed->code == sf::Keyboard::Key::Up) {
                    Logger::log("W key pressed.");

                    if(gameState == GameState::Playing) {
                        currentStage.addAction(Action::MoveUp);
                    }
                }

                else if(keyPressed->code == sf::Keyboard::Key::A || keyPressed->code == sf::Keyboard::Key::Left) {
                    Logger::log("A key pressed.");

                    if(gameState == GameState::Playing) {
                        currentStage.addAction(Action::MoveLeft);
                    }
                }

                else if(keyPressed->code == sf::Keyboard::Key::S || keyPressed->code == sf::Keyboard::Key::Down) {
                    Logger::log("S key pressed.");

                    if(gameState == GameState::Playing) {
                        currentStage.addAction(Action::MoveDown);
                    }
                }

                else if(keyPressed->code == sf::Keyboard::Key::D || keyPressed->code == sf::Keyboard::Key::Right) {
                    Logger::log("D key pressed.");

                    if(gameState == GameState::Playing) {
                        currentStage.addAction(Action::MoveRight);
                    }
                }

                else if(keyPressed->code == sf::Keyboard::Key::X) {
                    Logger::log("X key pressed.");

                    if(gameState == GameState::Playing) {
                        currentStage.addAction(Action::None);
                    }
                }

                else if(keyPressed->code == sf::Keyboard::Key::Backspace) {
                    Logger::log("Backspace key pressed.");

                    if(gameState == GameState::Playing) {
                        currentStage.undoLastAction();
                    }
                }
            }
        }



        // II: Handle
        if(gameState == GameState::Playing) {
            // if(isDragging) handleDrag(window, view, lastMousePos);

            if(stages.empty()) continue;
            Stage& currentStage = stages.at(stageIndex - 1);
            if(currentStage.reachMaxActions()) {
                currentStage.advance(gameState);
            }
        }
        


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
            stages.at(stageIndex - 1).draw(window, gameState);
        } else if(gameState == GameState::StageClear) {
            window.setView(view);
            stages.at(stageIndex - 1).draw(window, gameState);
        }

        // Update the window
        window.display();
    }



    // Cleanup and exit
    Logger::log("Game exited.");
    Logger::shutdown();

    return 0;
}