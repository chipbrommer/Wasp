#pragma once
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
#include "../external/nlohmann/json.hpp"       // json
#include "../utilities/json_file_utility.hpp"  // file utility type
//
/////////////////////////////////////////////////////////////////////////////////

/// @brief A structure representing a setting file
struct Settings : JsonType
{    
    // Communications
    std::string tmIpAddress = "224.1.1.1";
    int tmPort = 5700;
    int webPort = 8080;

    // Target
    double targetLatitude       = 0.0;
    double targetLongitude      = 0.0;
    double targetAltitudeHAE    = 0.0;
    double targetAltitudeMSL    = 0.0;

    // Test Flags
    bool testMode1              = false;

    // Logging
    std::string logFilePath = "";
    bool fileLoggingEnabled = false;

    /// @brief map for json item to variables
    std::unordered_map<std::string, std::function<void(const nlohmann::json&)>> jsonMapping
    {   
        {"tmIpAddress",         [this](const nlohmann::json& j) { j.at("tmIpAddress").get_to(tmIpAddress);                  }},
        {"tmPort",              [this](const nlohmann::json& j) { j.at("tmPort").get_to(tmPort);                            }},
        {"webPort",             [this](const nlohmann::json& j) { j.at("webPort").get_to(webPort);                          }},
        {"targetLatitude",      [this](const nlohmann::json& j) { j.at("targetLatitude").get_to(targetLatitude);            }},
        {"targetLongitude",     [this](const nlohmann::json& j) { j.at("targetLongitude").get_to(targetLongitude);          }},
        {"targetAltitudeHAE",   [this](const nlohmann::json& j) { j.at("targetAltitudeHAE").get_to(targetAltitudeHAE);      }},
        {"targetAltitudeMSL",   [this](const nlohmann::json& j) { j.at("targetAltitudeMSL").get_to(targetAltitudeMSL);      }},
        {"testMode1",           [this](const nlohmann::json& j) { j.at("testMode1").get_to(testMode1);                      }},
        {"logFilePath",         [this](const nlohmann::json& j) { j.at("logFilePath").get_to(logFilePath);                  }},
        {"fileLoggingEnabled",  [this](const nlohmann::json& j) { j.at("fileLoggingEnabled").get_to(fileLoggingEnabled);    }}
    };

    /// @brief Serialize structure to json
    /// @return json structure containing structure data
    nlohmann::json ToJson() const
    {
        return nlohmann::json {
            {"tmIpAddress",         tmIpAddress},
            {"tmPort",              tmPort},
            {"webPort",             webPort},
            {"targetLatitude",      targetLatitude},
            {"targetLongitude",     targetLongitude},
            {"targetAltitudeHAE",   targetAltitudeHAE},
            {"targetAltitudeMSL",   targetAltitudeMSL},
            {"testMode1",           testMode1},
            {"logFilePath",         logFilePath},
            {"fileLoggingEnabled",  fileLoggingEnabled}
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