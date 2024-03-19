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

struct Settings : JsonType
{
    // Ports
    std::string imuPort = "";
    int imuBaud = 11520;
    std::string gpsPort = "";
    int gpsBaud = 921600;
    
    // Target
    double latitude = 0.0;
    double longitude = 0.0;
    double altitudeHAE = 0.0;
    double altitudeMSL = 0.0;

    // Test
    bool testMode1 = false;

    /// @brief 
    std::unordered_map<std::string, std::function<void(const nlohmann::json&)>> jsonMapping
    {
        {"imuPort",     [this](const nlohmann::json& j) { j.at("imuPort").get_to(imuPort);          }},
        {"imuBaud",     [this](const nlohmann::json& j) { j.at("imuBaud").get_to(imuBaud);          }},
        {"gpsPort",     [this](const nlohmann::json& j) { j.at("gpsPort").get_to(gpsPort);          }},
        {"gpsBaud",     [this](const nlohmann::json& j) { j.at("gpsBaud").get_to(gpsBaud);          }},
        {"latitude",    [this](const nlohmann::json& j) { j.at("latitude").get_to(latitude);        }},
        {"longitude",   [this](const nlohmann::json& j) { j.at("longitude").get_to(longitude);      }},
        {"altitudeHAE", [this](const nlohmann::json& j) { j.at("altitudeHAE").get_to(altitudeHAE);  }},
        {"altitudeMSL", [this](const nlohmann::json& j) { j.at("altitudeMSL").get_to(altitudeMSL);  }},
        {"testMode1",   [this](const nlohmann::json& j) { j.at("testMode1").get_to(testMode1);      }},
    };

    /// @brief 
    /// @param j 
    void to_json(nlohmann::json& j) const
    {
        j = nlohmann::json{
            {"imuPort", imuPort},
            {"imuBaud", imuBaud},
            {"gpsPort", gpsPort},
            {"gpsBaud", gpsBaud},
            {"latitude", latitude},
            {"longitude", longitude},
            {"altitudeHAE", altitudeHAE},
            {"altitudeMSL", altitudeMSL},
            {"testMode1", testMode1}
        };
    }

    /// @brief 
    /// @param j 
    void from_json(const nlohmann::json& j) 
    {
        for (const auto& [key, func] : jsonMapping) 
        {
            if (j.contains(key)){ func(j); }
        }
    }
};