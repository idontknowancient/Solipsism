#include "Constants.hpp"
#include "Logger.hpp"
#include <fstream>
#include <sstream>

// Define static members with default values
bool Config::DEBUG_MODE = true;
int Config::WORLD_WIDTH = 1920;
int Config::WORLD_HEIGHT = 1080;
int Config::FRAME_RATE = 60;
float Config::BGM_VOLUME = 10.0f;
float Config::ZOOM_RATE = 0.1f;

void Config::init(const std::string& configFile) {
    std::ifstream file(configFile);
    if(!file.is_open()) {
        Logger::log("Warning: Config file '" + configFile + "' not found. Using default values.");
        return;
    }

    Logger::log("Loading configuration from " + configFile + "...");
    std::string line;
    int lineNum = 0;
    
    while(std::getline(file, line)) {
        lineNum++;
        
        // Skip empty lines and comments (lines starting with # or //)
        if(line.empty() || line[0] == '#' || (line.size() >= 2 && line[0] == '/' && line[1] == '/')) {
            continue;
        }
        
        // Find the '=' separator
        size_t equalPos = line.find('=');
        if(equalPos == std::string::npos) {
            continue; // Skip invalid lines
        }
        
        // Extract key and value
        std::string key = line.substr(0, equalPos);
        std::string value = line.substr(equalPos + 1);
        
        // Trim whitespace from key and value
        key.erase(0, key.find_first_not_of(" \t\r\n"));
        key.erase(key.find_last_not_of(" \t\r\n") + 1);
        value.erase(0, value.find_first_not_of(" \t\r\n"));
        value.erase(value.find_last_not_of(" \t\r\n") + 1);
        
        // Parse and set configuration values
        try {
            if(key == "DEBUG_MODE") {
                DEBUG_MODE = (value == "true" || value == "1");
                Logger::log("  DEBUG_MODE = " + std::string(DEBUG_MODE ? "true" : "false"));
            }
            else if(key == "WORLD_WIDTH") {
                WORLD_WIDTH = std::stoi(value);
                Logger::log("  WORLD_WIDTH = " + std::to_string(WORLD_WIDTH));
            }
            else if(key == "WORLD_HEIGHT") {
                WORLD_HEIGHT = std::stoi(value);
                Logger::log("  WORLD_HEIGHT = " + std::to_string(WORLD_HEIGHT));
            }
            else if(key == "FRAME_RATE") {
                FRAME_RATE = std::stoi(value);
                Logger::log("  FRAME_RATE = " + std::to_string(FRAME_RATE));
            }
            else if(key == "BGM_VOLUME") {
                BGM_VOLUME = std::stof(value);
                Logger::log("  BGM_VOLUME = " + std::to_string(BGM_VOLUME));
            }
            else if(key == "ZOOM_RATE") {
                ZOOM_RATE = std::stof(value);
                Logger::log("  ZOOM_RATE = " + std::to_string(ZOOM_RATE));
            }
        } catch(const std::exception& e) {
            Logger::log("Error parsing config line " + std::to_string(lineNum) + ": " + key + " = " + value);
        }
    }
    
    file.close();
    Logger::log("Configuration loaded successfully.");
}
