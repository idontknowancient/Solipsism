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

    // Settings Button
    RoundedRectangle settingsButton(
        BUTTON_CENTER_X, BUTTON_CENTER_Y + 100,
        BUTTON_RECTANGLE_WIDTH, BUTTON_RECTANGLE_HEIGHT,
        BUTTON_CIRCLE_RADIUS, BUTTON_SHADOW_OFFSET,
        "SETTINGS", 30, Resource::getButtonFont()
    );

    // Quit Button
    RoundedRectangle quitButton(
        BUTTON_CENTER_X, BUTTON_CENTER_Y + 200,
        BUTTON_RECTANGLE_WIDTH, BUTTON_RECTANGLE_HEIGHT,
        BUTTON_CIRCLE_RADIUS, BUTTON_SHADOW_OFFSET,
        "QUIT", 30, Resource::getButtonFont()
    );

    // Stage buttons - 2 rows, 5 columns, centered on screen
    std::vector<RoundedRectangle> stageButtons;
    float buttonWidth = 140;
    float buttonHeight = 60;
    float spacing = 140.f; // Space between buttons
    
    // Calculate grid dimensions
    float gridWidth = 5 * buttonWidth + 4 * spacing;
    float gridHeight = 2 * buttonHeight + 1 * spacing;
    float startX = (WORLD_WIDTH - gridWidth) / 2.f;
    float startY = (WORLD_HEIGHT - gridHeight) / 2.f;
    
    for(int i = 0; i < 10; i++) {
        int row = i / 5;
        int col = i % 5;
        float x = startX + col * (buttonWidth + spacing);
        float y = startY + row * (buttonHeight + spacing);
        
        stageButtons.emplace_back(
            x + buttonWidth / 2.f, y + buttonHeight / 2.f,
            buttonWidth, buttonHeight,
            BUTTON_CIRCLE_RADIUS, BUTTON_SHADOW_OFFSET,
            std::to_string(i + 1), 40, Resource::getButtonFont()
        );
    }



    // Gamestate
    GameState gameState = GameState::TitleScreen;
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
                // Keep view fixed to world size to prevent stretching ...?
                // The viewport will add black bars as needed
                window.setView(sf::View(sf::FloatRect({0.f, 0.f}, {WORLD_WIDTH, WORLD_HEIGHT})));
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
                            gameState = GameState::StageSelect;
                            Logger::log("Start Game button pressed. Entering Stage Select state.");
                        } else if(settingsButton.isClicked(worldPos)) {
                            Logger::log("Settings button pressed. (No action implemented)");
                        } else if(quitButton.isClicked(worldPos)) {
                            Resource::getMusic().stop();
                            window.close();
                            Logger::log("Quit button pressed. Exiting game.");
                        }
                    }

                    else if(gameState == GameState::StageSelect) {
                        // Map pixel to world coordinates
                        sf::Vector2f worldPos = window.mapPixelToCoords(mousePressed->position);

                        // Check if stage buttons are pressed
                        for(int i = 0; i < stageButtons.size(); i++) {
                            if(stageButtons[i].isClicked(worldPos)) {
                                if(i >= stages.size()) {
                                    Logger::log("Stage " + std::to_string(i + 1) + " does not exist. Staying in Stage Select.");
                                    break;
                                }

                                gameState = GameState::Playing;
                                stageIndex = i + 1;
                                Logger::log("Stage " + std::to_string(stageIndex) + " button pressed. Entering Playing state.");
                                break;
                            }
                        }
                    }

                    else if(gameState == GameState::Playing) {
                        isDragging = true;
                    }

                    else if(gameState == GameState::StageClear) {
                        // Map pixel to world coordinates
                        sf::Vector2f worldPos = window.mapPixelToCoords(mousePressed->position);

                        if(Stage::buttonSelect.isClicked(worldPos)) {
                            Logger::log("SELECT button pressed. Returning to Stage Select.");
                            stages.at(stageIndex - 1).reset();
                            gameState = GameState::StageSelect;
                        } else if(Stage::buttonRetry.isClicked(worldPos)) {
                            Logger::log("RETRY button pressed. Restarting Stage " + std::to_string(stageIndex) + ".");
                            stages.at(stageIndex - 1).reset();
                            gameState = GameState::Playing;
                        } else if(Stage::buttonNext.isClicked(worldPos)) {
                            if(stageIndex + 1 > stages.size()) {
                                Logger::log("NEXT button pressed but no more stages. Staying on the screen.");
                                break;
                            } else {
                                Logger::log("NEXT button pressed. Proceeding to Stage " + std::to_string(stageIndex + 1) + ".");
                                stages.at(stageIndex - 1).reset();
                                stageIndex++;
                            }
                            gameState = GameState::Playing;
                        }
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
                    
                    // Adjust music volume
                    Resource::getMusic().setVolume(
                        clamp(Resource::getMusic().getVolume() + mouseWheel->delta * 5.f, 0.f, 100.f)
                    );
                }
            }

            else if(const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {
                if(keyPressed->code == sf::Keyboard::Key::Escape) {
                    Logger::log("Escape key pressed.");

                    if(gameState == GameState::Playing) {
                        Logger::log("Returning to Stage Select.");
                        gameState = GameState::StageSelect;
                    } else if(gameState == GameState::StageSelect) {
                        Logger::log("Returning to Title Screen from Stage Select.");
                        gameState = GameState::TitleScreen;
                    } else if(gameState == GameState::StageClear) {
                        stages.at(stageIndex - 1).reset();
                        Logger::log("Returning to Stage Select from Stage Clear.");
                        gameState = GameState::StageSelect;
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
                    } else if(gameState == GameState::TitleScreen) {
                        gameState = GameState::StageSelect;
                        Logger::log("Start Game button pressed. Entering Stage Select state.");
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
        } else if(gameState == GameState::StageSelect) {
            window.setView(view);
            window.draw(backgroundSprite);

            // Draw stage buttons
            for(auto& button : stageButtons) {
                button.draw(window);
            }
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