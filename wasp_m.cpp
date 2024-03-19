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

Wasp::Wasp(const std::string& settingsLocation, const std::string& buildLocation) : 
    settingsFile(), settings(settingsLocation), buildFile(), build(buildLocation) 
{
    if (!settings.Load())
    {
        std::cerr << "[WASP Error] - Failed to Load/Create settings file.\n";
    }

    if (!build.Load())
    {
        std::cerr << "[WASP Error] - Failed to Load/Create build file.\n";
    }
}

Wasp::~Wasp() {}

void Wasp::Execute()
{
    std::cout << "Welcome to wasp.\n";
}