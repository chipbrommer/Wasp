#pragma once
/////////////////////////////////////////////////////////////////////////////////
// @file            log_client.h
// @brief           A multi-level static log client with file capabilites
// @author          Chip Brommer
/////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////
//
// Includes:
//          name                            reason included
//          ------------------              ------------------------
#include <string>                           // strings
#include <iostream>                         // console io
#include <fstream>                          // file io
#include <exception>                        // exceptions
#include <filesystem>                       // files
#include <queue>                            // queue
#include <mutex>                            // mutex
#include <map>
//
/////////////////////////////////////////////////////////////////////////////////

class LogClient
{
public:
    /// @brief enum for acceptable log levels
    enum class LogLevel
    {
        Debug,
        Info,
        Warning,
        Error,
    };

    /// @brief Default Constructor
    /// @param logFilePath - opt - filepath for desired log file to enable logging to file
    LogClient(const std::string& logFilePath = "")
        : m_name("LOGGER"), mLogQueue(), mQueueMutex(), mFileLoggingEnabled(false),
        mLogFile(logFilePath)
    {
        // If we received a logFilePath, attempt to enable file logging. 
        if (!logFilePath.empty()) { EnableFileLogging(logFilePath); }
    }

    /// @brief Default Deconstructor
    ~LogClient();

    /// @brief Add a log to the queue to be logged
    /// @param name - opt - Name of the sender / area / class writing the log
    /// @param level - level of the log item
    /// @param message - formatted message to be logged
    void AddLog(const std::string& name, const LogLevel level, const std::string& message);

    /// @brief Enable the logger to write the log to a file
    /// @param filename - desired file location and name 
    /// @return true if logging enabled and file opened/created successfully.
    bool EnableFileLogging(const std::string& filename);

    /// @brief Main working loop to log items. BLOCKING. Meant to be called in a thread.
    void Run();

    /// @brief Stop the main running loop. 
    /// @param waitForEmptyQueue - opt - Flag to wait to Stop until queue is empty
    void Stop(bool waitForEmptyQueue = false);

    /// @brief Clear the log queue of all items
    void ClearLogQueue();

protected:

private:
    /// @brief Convenience mapping for LogLevel to string value
    const std::map<LogClient::LogLevel, std::string> LogLevelToStringMap = {
        {LogClient::LogLevel::Debug,    "DEBUG"},
        {LogClient::LogLevel::Info,     "INFO"},
        {LogClient::LogLevel::Warning,  "WARNING"},
        {LogClient::LogLevel::Error,    "ERROR"}
    };

    /// @brief A struct to represent a log
    struct LogItem
    {
        double timestamp;
        LogLevel level;
        std::string message;
    };

    std::string m_name;             /// name of the logger when writing logs
    std::queue<LogItem> mLogQueue;  /// queue of log items 
    std::mutex mQueueMutex;         /// mutex for queue protection
    bool mFileLoggingEnabled;       /// flag for file logging being enabled
    std::ofstream mLogFile;         /// filestream for the log file
    std::atomic_bool mRun;          /// bool for a run flag

    /// @brief 
    /// @param name 
    /// @param level 
    /// @param message 
    /// @return 
    std::string CreateLogString(const std::string& name, const std::string& level, const std::string& message);

    /// @brief 
    /// @param log 
    void WriteLog(const LogItem log);
};