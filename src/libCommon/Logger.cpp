#include "Logger.h"
#include <cassert>
#include <fstream>
#include <filesystem>
#include <format>
#include <chrono>

namespace fs = std::filesystem;

void Logger::print(LogLevel level, const std::string& message)
{
    if (level < _currentLogLevel)
        return;
    
    auto result = _logQueue.enqueue(LogData{
                                        level, 
                                        message, 
                                        __TIMESTAMP__, 
                                        std::this_thread::get_id(), 
                                        __FILE_NAME__, 
                                        __LINE__});
    if (UNLIKELY(!result.has_value())) 
    {
        assert(false && "Logger: Log queue is full, message dropped");
    }
}

void Logger::setLogLevel(LogLevel level)
{
    _currentLogLevel = level;
}

LogLevel Logger::GetLogLevel() const
{
    return _currentLogLevel;
}

void Logger::writeLogToFile(std::ofstream& logFile, const LogData& logEntry)
{
    switch (logEntry.level) {
        case LogLevel::TRACE:
            logFile << "[TRACE] ";
            break;
        case LogLevel::DEBUG:
            logFile << "[DEBUG] ";
            break;
        case LogLevel::INFO:
            logFile << "[INFO] ";
            break;
        case LogLevel::WARN:
            logFile << "[WARN] ";
            break;
        case LogLevel::ERROR:
            logFile << "[ERROR] ";
            break;
        default:
            break;
    }
    logFile << "[" << logEntry.timestamp << "] "
              << "[" << logEntry.threadId << "] "
              << logEntry.message << " ("
              << logEntry.fileName << ":"
              << logEntry.lineNumber << ")\n";

    logFile.flush();
}

std::string Logger::getLogFileName() const
{
    return _logFileName;
}

void Logger::prepareLogFile()
{
    std::string logDir = std::string(SOURCE_ROOT) + "/logs/";
    std::string archiveDir = logDir + "/archive/";
    fs::exists(logDir) || fs::create_directories(logDir);
    fs::exists(archiveDir) || fs::create_directories(archiveDir);

    std::string fileName(LOG_FILE_NAME);
    if (fileName.empty()) {
        fileName = "application";
    }

    using namespace std::chrono;
    auto now = floor<seconds>(system_clock::now());
    std::string ts = std::format("{:%Y%m%d_%H%M%S}", now);

    _logFileName =  logDir + fileName + "_" + ts + ".log";

}
Logger::Logger()
{   
    // Initialize log file name, create directories if needed
    prepareLogFile();

    _writerThread = std::jthread([this](std::stop_token st){

        std::ofstream logFile(_logFileName, std::ios::app);

        if (!logFile.is_open()) {
            assert(false && "Logger: Unable to open log file for writing");
            return;
        }

        logFile << "----- Logger Started -----\n";
        
        LogData logEntry;
        while (!st.stop_requested()) {
            auto result = _logQueue.dequeue(logEntry);
            if (result.has_value()) {
                writeLogToFile(logFile, logEntry);  
            } else {
                // Sleep briefly to avoid busy-waiting
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
            }
        }
        logFile << "Logger stopping, flushing remaining log entries...\n";

        // Drain remaining log entries
        while (_logQueue.dequeue(logEntry).has_value()) {
            writeLogToFile(logFile, logEntry);
        }

        logFile << "----- Logger Stopped -----\n";
        logFile.close();
    });
}

Logger::~Logger()
{
}