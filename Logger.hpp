#pragma once
#include "Constants.hpp"
#include <string>
#include <fstream>

class Logger {
public:
    // Initialize the logging system (invoke at the start of main)
    static void init(const std::string& filename = "debug_log.txt");

    // Close the logging system (invoke at the end of main)
    static void shutdown();

    // Log a message with a timestamp
    static void log(const std::string& message);
    // Log a debug message (only if DEBUG_MODE is true)
    static void log_debug(const std::string& message);

private:
    static std::ofstream logFile;
    static bool initialized;

    static std::string getCurrentTime();

    // Hide constructors to prevent instantiation
    Logger() = delete;
    ~Logger() = delete;
};