#include <gtest/gtest.h>
#include <fstream>
#include <filesystem>
#include <string>
#include "Logger.h"

namespace fs = std::filesystem;

TEST(LoggerTest, LogFileGeneratedAndContent)
{
    // count to differentiate logs printed in each loop
    int i=0;
    for (auto& level : {LogLevel::TRACE, LogLevel::DEBUG, LogLevel::INFO, LogLevel::WARN, LogLevel::ERROR}) {
        i++;
        std::string logFileName = Logger::getInstance().getLogFileName();
        Logger::getInstance().setLogLevel(level);

        // Only relevant logs should be printed based on the log level
        LOG_TRACE("TRACELOG" + std::to_string(i));
        LOG_DEBUG("DEBUGLOG" + std::to_string(i));
        LOG_INFO("INFOLOG" + std::to_string(i));
        LOG_WARN("WARNLOG" + std::to_string(i));
        LOG_ERROR("ERRORLOG" + std::to_string(i));

        // Allow some time for the logger thread to process messages
        std::this_thread::sleep_for(std::chrono::milliseconds(100));

        // Check if log file is created
        std::ifstream logFile(logFileName);
        ASSERT_TRUE(logFile.is_open()) << "Log file was not created.";
        // Check if log file contains the logged messages
        std::string line;
        bool foundTrace = false, foundDebug = false, foundInfo = false, foundWarn
            = false, foundError = false;
        while (std::getline(logFile, line)) {
            if (line.find("TRACELOG" + std::to_string(i)) != std::string::npos)
                foundTrace = true;
            if (line.find("DEBUGLOG" + std::to_string(i)) != std::string::npos)
                foundDebug = true;
            if (line.find("INFOLOG" + std::to_string(i)) != std::string::npos)
                foundInfo = true;
            if (line.find("WARNLOG" + std::to_string(i)) != std::string::npos)
                foundWarn = true;
            if (line.find("ERRORLOG" + std::to_string(i)) != std::string::npos)
                foundError = true;
        }

        switch (level)
        {
        case LogLevel::TRACE:
            EXPECT_TRUE(foundTrace & foundDebug & foundInfo & foundWarn & foundError);
            break;
        case LogLevel::DEBUG:
            EXPECT_TRUE(foundDebug & foundInfo & foundWarn & foundError);
            EXPECT_TRUE(!foundTrace);
            break;
        case LogLevel::INFO:
            EXPECT_TRUE(foundInfo & foundWarn & foundError);
            EXPECT_TRUE(!foundTrace & !foundDebug);
            break;
        case LogLevel::WARN:
            EXPECT_TRUE(foundWarn & foundError);
            EXPECT_TRUE(!foundTrace & !foundDebug & !foundInfo);
            break;
        case LogLevel::ERROR:
            EXPECT_TRUE(foundError);
            EXPECT_TRUE(!foundTrace & !foundDebug & !foundInfo & !foundWarn);
            break;
        default:
            break;
        }
    
    }
}

