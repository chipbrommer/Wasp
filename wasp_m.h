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
#include "settings.h"                       // program settings
#include "build.h"                          // build information
#include "configuration.h"                  // configuration information
#include "utilities/json_file_utility.hpp"  // settings utility
#include "utilities/pwm_interface.h"        // pwms
// 
/////////////////////////////////////////////////////////////////////////////////

/// @brief Wasp Munition
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
    JsonFileUtility<Settings> mSettings;
    JsonFileUtility<Build> mBuild;
    JsonFileUtility<Configuration> mConfig;
    
    PWM mFin1;
    PWM mFin2;
    PWM mFin3;
    PWM mFin4;

};