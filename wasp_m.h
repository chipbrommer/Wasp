/////////////////////////////////////////////////////////////////////////////////
// @file            wasp_m.h
// @brief           Main application
// @author          Chip Brommer
/////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////
//
// Includes:
//          name                            reason included
//          ------------------              ------------------------
#include <string>                           // strings
#include <cstdint>                          // standard ints
//
#include "files/settings.h"                 // program settings
#include "files/build.h"                    // build information
#include "files/configuration.h"            // configuration information
#include "utilities/json_file_utility.hpp"  // settings utility
#include "signal_manager.h"                 // signal manager
// 
/////////////////////////////////////////////////////////////////////////////////

class Wasp
{
public:
    /// @brief Default constructure
    /// @param settingsLocation - Location of the settings file
    /// @param buildLocation - Location of the build file
    /// @param configLocation - Location of the configuration file
    Wasp(const std::string& settingsLocation, const std::string& buildLocation, const std::string& configLocation);

    /// @brief Default deconstructor
    ~Wasp();

    /// @brief Main entry point to start Wasp
    void Execute();

protected:

private:
    JsonFileUtility<Settings> mSettings;        /// Settings file 
    JsonFileUtility<Build> mBuild;              /// Build log file
    JsonFileUtility<Configuration> mConfig;     /// Configuration file
    
    SignalManager mSignals;                     /// Manager for PWMs and GPIOs
};