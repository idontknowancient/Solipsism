#include "Logger.hpp"
#include "Constants.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <chrono>

// Initialize static members
std::ofstream Logger::logFile;
bool Logger::initialized = false;

void Logger::init(const std::string& filename) {
    if(initialized) return;

    // Use truncation to start a new log file each time
    logFile.open(filename, std::ios::trunc);
    
    if(logFile.is_open()) {
        initialized = true;
        log("--- Logger Initialized ---");
    } else {
        std::cerr << "ERROR: Failed to open log file: " << filename << std::endl;
    }
}

void Logger::shutdown() {
    if(initialized && logFile.is_open()){
        log("--- Logger Shutdown ---");
        logFile.close();
        initialized = false;
    }
}

std::string Logger::getCurrentTime() {
    // Use chrono to get current time
    auto now = std::chrono::system_clock::now();
    auto in_time_t = std::chrono::system_clock::to_time_t(now);
    
    // Use put_time to format the time
    std::stringstream ss;
    ss << "[" << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d %H:%M:%S") << "] ";
    return ss.str();
}

void Logger::log(const std::string& message) {
    if(initialized) {
        std::string timeStamp = getCurrentTime();
        
        // Write to log file
        logFile << timeStamp << message << std::endl;
    }
}

void Logger::log_debug(const std::string& message) {
    if(initialized && DEBUG_MODE) {
        log(message);
    }
}