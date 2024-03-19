/////////////////////////////////////////////////////////////////////////////////
// @file            settings.h
// @brief           Structure for the programs settings
// @author          Chip Brommer
/////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////
//
// Includes:
//          name                            reason included
//          ------------------              ------------------------
#include <string>                           // strings
#include <cstdint>                          // standard ints
#include <map>                              // unordered map
//
#include "external/nlohmann/json.hpp"       // json
#include "utilities/json_file_utility.hpp"  // file utility type
//
/////////////////////////////////////////////////////////////////////////////////

/// @brief A structure representing a setting file
struct Settings : JsonType
{    
    // Target
    double targetLatitude   = 0.0;
    double targetLongitude  = 0.0;
    double altitudeHAE      = 0.0;
    double altitudeMSL      = 0.0;

    // Test Flags
    bool testMode1          = false;

    /// @brief map for json item to variables
    std::unordered_map<std::string, std::function<void(const nlohmann::json&)>> jsonMapping
    {
        {"targetLatitude",  [this](const nlohmann::json& j) { j.at("targetLatitude").get_to(targetLatitude);    }},
        {"targetLongitude", [this](const nlohmann::json& j) { j.at("targetLongitude").get_to(targetLongitude);  }},
        {"altitudeHAE",     [this](const nlohmann::json& j) { j.at("altitudeHAE").get_to(altitudeHAE);          }},
        {"altitudeMSL",     [this](const nlohmann::json& j) { j.at("altitudeMSL").get_to(altitudeMSL);          }},
        {"testMode1",       [this](const nlohmann::json& j) { j.at("testMode1").get_to(testMode1);              }},
    };

    /// @brief Serialize structure to json
    /// @return json structure containing structure data
    nlohmann::json ToJson() const
    {
        return nlohmann::json {
            {"targetLatitude",  targetLatitude},
            {"targetLongitude", targetLongitude},
            {"altitudeHAE",     altitudeHAE},
            {"altitudeMSL",     altitudeMSL},
            {"testMode1",       testMode1}
        };
    }

    /// @brief Mandatory function for serializing settings to json
    /// @param j - out - json object containing settings
    void ToJson(nlohmann::json& j) const
    {
        j = ToJson();
    }

    /// @brief Mandatory function for deserializing settings from json
    /// @param j - in - json object containing settings
    void FromJson(const nlohmann::json& j) 
    {
        for (const auto& [key, func] : jsonMapping) 
        {
            if (j.contains(key)){ func(j); }
        }
    }
};