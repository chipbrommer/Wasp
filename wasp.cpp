/////////////////////////////////////////////////////////////////////////////////
// @file            wasp_m.cpp
// @brief           Wasp implementation
// @author          Chip Brommer
/////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////
//
// Includes:
//          name                            reason included
//          ------------------              ------------------------
#include "wasp.h"                           // Header
// 
/////////////////////////////////////////////////////////////////////////////////

Wasp::Wasp(const std::string& settingsLocation, const std::string& buildLocation, const std::string& configLocation) : 
    mSettings(settingsLocation), mBuild(buildLocation), mConfig(configLocation), 
    mName("WASP"), mLogger(), mSignalManger(mLogger)
{
    // Load the configs and catch any failures
    if (!mSettings.Load())
    {
        std::cerr << "[WASP] - ERROR - Failed to Load/Create settings file.\n";
    }

    if (!mBuild.Load())
    {
        std::cerr << "[WASP] - ERROR - Failed to Load/Create build file.\n";
    }

    if (!mConfig.Load())
    {
        std::cerr << "[WASP] - ERROR - Failed to Load/Create config file.\n";
    }

    // Start the logger
    if (mConfig.data.fileLoggingEnabled)
    {
        mLogger.EnableFileLogging(mConfig.data.logFilePath);
    }
    mLoggingThread = std::thread([this] { mLogger.Run(); });

    // Initialize the signal managers PWMs
    mLogger.AddLog(mName, LogClient::LogLevel::INFO, "Starting Signal Manager.");
    mSignalManger.ReadyFin(SignalManager::FIN::ONE,      mConfig.data.fin1Path, mConfig.data.fin1Channel, mConfig.data.finMinDegrees, mConfig.data.finMaxDegrees);
    mSignalManger.ReadyFin(SignalManager::FIN::TWO,      mConfig.data.fin2Path, mConfig.data.fin2Channel, mConfig.data.finMinDegrees, mConfig.data.finMaxDegrees);
    mSignalManger.ReadyFin(SignalManager::FIN::THREE,    mConfig.data.fin3Path, mConfig.data.fin3Channel, mConfig.data.finMinDegrees, mConfig.data.finMaxDegrees);
    mSignalManger.ReadyFin(SignalManager::FIN::FOUR,     mConfig.data.fin4Path, mConfig.data.fin4Channel, mConfig.data.finMinDegrees, mConfig.data.finMaxDegrees);
    mSignalThread = std::thread([this] {mSignalManger.Start(); });

}

Wasp::~Wasp() 
{
    mSignalManger.Stop();
    if (mSignalThread.joinable()) mSignalThread.join();

    // Close the logger last but wait until all logs have been written
    mLogger.Stop(true);
    if (mLoggingThread.joinable()) mLoggingThread.join();
}

void Wasp::Execute()
{


    std::cout << "Welcome to wasp.\n";

    mLogger.AddLog(mName, LogClient::LogLevel::INFO, "Welcome!");

    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    mLogger.AddLog(mName, LogClient::LogLevel::ERROR, "Error Test!");

    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    mLogger.AddLog(mName, LogClient::LogLevel::WARNING, "Warning Test!");

    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    mLogger.AddLog(mName, LogClient::LogLevel::INFO, "Closing!");

    std::this_thread::sleep_for(std::chrono::milliseconds(100));
}