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

class Wasp
{
public:
    /// @brief 
    /// @param settingsLocation 
    /// @param buildLocation 
    Wasp(const std::string& settingsLocation, const std::string& buildLocation);

    /// @brief 
    ~Wasp();

    /// @brief 
    void Execute();

protected:

private:
    Settings settingsFile;
    JsonFileUtility<Settings> settings;
    Build buildFile;
    JsonFileUtility<Build> build;
};