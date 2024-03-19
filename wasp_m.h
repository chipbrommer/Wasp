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
    JsonFileUtility<Settings> mSettings;        /// Settings file 
    JsonFileUtility<Build> mBuild;              /// Build log file
    JsonFileUtility<Configuration> mConfig;     /// Configuration file
    
    PWM mFin1;                                  /// PWM connected to Fin 1
    PWM mFin2;                                  /// PWM connected to Fin 2
    PWM mFin3;                                  /// PWM connected to Fin 3
    PWM mFin4;                                  /// PWM connected to Fin 4

};