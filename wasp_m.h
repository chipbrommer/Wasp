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
#include "utilities/json_file_utility.hpp"  // settings utility
// 
/////////////////////////////////////////////////////////////////////////////////

/// @brief Wasp Munition
class Wasp
{
public:
    /// @brief Default constructure
    /// @param settingsLocation - Location of the settings file
    /// @param buildLocation - Location of the build file
    Wasp(const std::string& settingsLocation, const std::string& buildLocation);

    /// @brief Default deconstructor
    ~Wasp();

    /// @brief Main entry point to start Wasp
    void Execute();

protected:

private:
    Settings settingsFile;
    JsonFileUtility<Settings> settings;
    Build buildFile;
    JsonFileUtility<Build> build;
};