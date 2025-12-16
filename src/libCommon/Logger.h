#pragma once

#include <string>
#include <thread>
#include "LockFreeQueue.hpp"

static const size_t LOG_MESSAGE_MAX_LENGTH = 1024; // 24 KB

enum class LogLevel
{
    TRACE,
    DEBUG,
    INFO,
    WARN,
    ERROR
};
#define LOG_TRACE(MSG) \
    do { \
        if (Logger::getInstance().GetLogLevel() <= LogLevel::TRACE) { \
            Logger::getInstance().print(LogLevel::TRACE, MSG); \
        } \
    } while (0)
#define LOG_DEBUG(MSG) \
    do { \
        if (Logger::getInstance().GetLogLevel() <= LogLevel::DEBUG) { \
            Logger::getInstance().print(LogLevel::DEBUG, MSG); \
        } \
    } while (0)
#define LOG_INFO(MSG) \
    do { \
        if (Logger::getInstance().GetLogLevel() <= LogLevel::INFO) { \
            Logger::getInstance().print(LogLevel::INFO, MSG); \
        } \
    } while (0)
#define LOG_WARN(MSG) \
    do { \
        if (Logger::getInstance().GetLogLevel() <= LogLevel::WARN) { \
            Logger::getInstance().print(LogLevel::WARN, MSG); \
        } \
    } while (0)

#define LOG_ERROR(MSG) \
    do { \
        if (Logger::getInstance().GetLogLevel() <= LogLevel::ERROR) { \
            Logger::getInstance().print(LogLevel::ERROR, MSG); \
        } \
    } while (0)



class Logger {

    struct LogData 
    {
        LogLevel level;
        std::string message;
        std::string timestamp;
        std::__thread_id threadId;
        std::string fileName;
        int lineNumber;
    };
    
    std::string _logFileName;
    LockFreeQueue<LogData, LOG_MESSAGE_MAX_LENGTH> _logQueue;
    LogLevel _currentLogLevel = LogLevel::INFO;
    std::jthread _writerThread;

    Logger();
    void writeLogToFile(std::ofstream& logFile, const LogData& logEntry);
    void prepareLogFile();
public:
    
    void setLogLevel(LogLevel level);
    LogLevel GetLogLevel() const;
    void print(LogLevel level, const std::string& message);
    static Logger& getInstance()
    {
        static Logger instance;
        return instance;
    }

    std::string getLogFileName() const;

    //delete copy/move constructors and assignment operators
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;
    Logger(Logger&&) = delete;
    Logger& operator=(Logger&&) = delete;
    ~Logger();
};