
/////////////////////////////////////////////////////////////////////////////////
// @file            log_client.cpp
// @brief           Implementation for the LogClient static logger
// @author          Chip Brommer
/////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////
//
// Includes:
//          name                            reason included
//          ------------------              ------------------------
#include "log_client.h"                     // header
//
/////////////////////////////////////////////////////////////////////////////////

LogClient::~LogClient()
{
    Stop();
}

void LogClient::AddLog(const std::string& name, const LogLevel level, const std::string& message)
{
    // Make sure the level is within scope
    if (static_cast<int>(level) < 0 || static_cast<int>(level) >= LogLevelToStringMap.size())
    {
        // Log an error if the log level is out of range
        WriteLog({ 0, LogLevel::Error, "Invalid log level: " + std::to_string(static_cast<int>(level)) + " from " + name });
        return;
    }

    mLogQueue.push({ 0, level, CreateLogString(name, LogLevelToStringMap.at(level), message) });
}

bool LogClient::EnableFileLogging(const std::string& filename)
{
    // Check if already enabled
    if (mFileLoggingEnabled) return true;

    // Catch a bad filename
    if (filename.empty()) return false;

    // Check if the filename has an extension. If not append '.log' to it. 
    std::string file = filename;
    if (file.find('.') == std::string::npos) 
    {
        file += ".log";
    }

    // Attempt to open the file
    mLogFile.open(file);
    if (!mLogFile.is_open()) 
    {
        WriteLog({ 0, LogLevel::Error, "Failed to open log file: " + file });
        return false;
    }

    // Log file successfully enabled
    mFileLoggingEnabled = true;

    // Write a notice
    WriteLog({0, LogLevel::Info, 
        CreateLogString(m_name, LogLevelToStringMap.at(LogLevel::Info), 
            "File Logging Enabled at: " + filename) }
    );
    
    return true;
}

void LogClient::Run()
{
    mRun = true;

    while (mRun)
    {
        // Check if there are logs in the queue
        if (!mLogQueue.empty())
        {
            std::scoped_lock lock(mQueueMutex);

            LogItem log = mLogQueue.front();
            WriteLog(log);
            mLogQueue.pop();
        }

        // Rest
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

void LogClient::Stop(bool waitForEmptyQueue)
{
    // Create a notice and write it. 
    WriteLog({ 0, LogLevel::Info, CreateLogString(m_name, LogLevelToStringMap.at(LogLevel::Info), "Stopping.") });

    if (mRun)
    {
        if (waitForEmptyQueue)
        {
            while (!mLogQueue.empty())
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
            }
        }

        // Signal the run to stop
        mRun = false;

        // Clear the queue
        ClearLogQueue();
    }
}

void LogClient::ClearLogQueue()
{
    // Create a notice and write it. 
    WriteLog({0, LogLevel::Info, CreateLogString(m_name, LogLevelToStringMap.at(LogLevel::Info), "Clearing Queue.") });
    
    // Clear the queue
    std::scoped_lock lock(mQueueMutex);
    std::queue<LogItem> empty;
    size_t numInQueue = mLogQueue.size();
    std::swap(mLogQueue, empty);

    // Notify the number of cleared items. 
    std::string temp = "Cleared " + std::to_string(numInQueue) + " logs.";
    WriteLog({0, LogLevel::Info, CreateLogString(m_name, LogLevelToStringMap.at(LogLevel::Info), temp) });
}

std::string LogClient::CreateLogString(const std::string& name, const std::string& level, const std::string& message)
{
    std::string temp = "[" + name + "]" + " - " + level + " - " + message;
    return temp;
}

void LogClient::WriteLog(const LogItem log)
{
    switch (log.level)
    {
        // Print to console as basic color
    case LogLevel::Debug:
    case LogLevel::Info:
        std::cout << log.message << "\n";
        break;
        // print to console as yellow
    case LogLevel::Warning:
        std::cout << "\033[33m" << log.message << "\033[0m\n";
        break;
    case LogLevel::Error:
        std::cout << "\033[31m" << log.message << "\033[0m\n";
        break;
    }

    if (mFileLoggingEnabled && mLogFile.is_open())
    {
        mLogFile << log.message << "\n";
    }
}