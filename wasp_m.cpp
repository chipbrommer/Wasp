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
    mSettings(settingsLocation), mBuild(buildLocation), mConfig(configLocation), 
    mFin1(""), mFin2(""), mFin3(""), mFin4("")
{
    // Load the configs and catch any failures
    if (!mSettings.Load())
    {
        std::cerr << "[WASP Error] - Failed to Load/Create settings file.\n";
    }

    if (!mBuild.Load())
    {
        std::cerr << "[WASP Error] - Failed to Load/Create build file.\n";
    }

    if (!mConfig.Load())
    {
        std::cerr << "[WASP Error] - Failed to Load/Create config file.\n";
    }

    // Initialize the PWMs
    Configuration config = mConfig.GetData();
    mFin1 = PWM(config.fin1Path, config.fin1Channel, config.finMinDegrees, config.finMaxDegrees);
    mFin2 = PWM(config.fin2Path, config.fin2Channel, config.finMinDegrees, config.finMaxDegrees);
    mFin3 = PWM(config.fin3Path, config.fin3Channel, config.finMinDegrees, config.finMaxDegrees);
    mFin4 = PWM(config.fin4Path, config.fin4Channel, config.finMinDegrees, config.finMaxDegrees);
}

Wasp::~Wasp() {}

void Wasp::Execute()
{
    std::cout << "Welcome to wasp.\n";
}