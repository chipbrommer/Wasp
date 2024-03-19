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
#include "wasp_m.h"                         // Header
// 
/////////////////////////////////////////////////////////////////////////////////

Wasp::Wasp(const std::string& settingsLocation, const std::string& buildLocation, const std::string& configLocation) : 
    mSettings(settingsLocation), mBuild(buildLocation), mConfig(configLocation), mSignals()
{
    // Load the configs and catch any failures
    if (!mSettings.Load())
    {
        std::cerr << "[WASP] - Error - Failed to Load/Create settings file.\n";
    }

    if (!mBuild.Load())
    {
        std::cerr << "[WASP] - Error - Failed to Load/Create build file.\n";
    }

    if (!mConfig.Load())
    {
        std::cerr << "[WASP] - Error - Failed to Load/Create config file.\n";
    }

    // Initialize the utilities
    mSignals = SignalManager(mConfig.data.fin1Path, mConfig.data.fin1Channel,
                            mConfig.data.fin2Path, mConfig.data.fin2Channel,
                            mConfig.data.fin3Path, mConfig.data.fin3Channel,
                            mConfig.data.fin4Path, mConfig.data.fin4Channel,
                            mConfig.data.finMinDegrees, mConfig.data.finMaxDegrees);

}

Wasp::~Wasp() {}

void Wasp::Execute()
{
    std::cout << "Welcome to wasp.\n";
}