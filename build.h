/////////////////////////////////////////////////////////////////////////////////
// @file            build.h
// @brief           Structure for the device build information
// @author          Chip Brommer
/////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////
//
// Includes:
//          name                            reason included
//          ------------------              ------------------------
#include <string>                           // strings
//
#include "external/nlohmann/json.hpp"       // json
#include "utilities/json_file_utility.hpp"  // file utility type
//
/////////////////////////////////////////////////////////////////////////////////

struct Build : JsonType
{
    std::string imuSerialNumber = "";
    std::string gpsSerialNumber = "";
    std::string osVersion = "";

    /// @brief 
    std::unordered_map<std::string, std::function<void(const nlohmann::json&)>> jsonMapping
    {
        {"imuSerialNumber", [this](const nlohmann::json& j) { j.at("imuSerialNumber").get_to(imuSerialNumber);  }},
        {"gpsSerialNumber", [this](const nlohmann::json& j) { j.at("gpsSerialNumber").get_to(gpsSerialNumber);  }},
        {"osVersion",       [this](const nlohmann::json& j) { j.at("osVersion").get_to(osVersion);              }}
    };

    /// @brief 
    /// @param j 
    void to_json(nlohmann::json& j) const
    {
        j = nlohmann::json{ 
            {"imuSerialNumber", imuSerialNumber},
            {"gpsSerialNumber", gpsSerialNumber},
            {"osVersion", osVersion} 
        };
    }

    /// @brief 
    /// @param j 
    void from_json(const nlohmann::json& j)
    {
        j.at("imuSerialNumber").get_to(imuSerialNumber);
        j.at("gpsSerialNumber").get_to(gpsSerialNumber);
        j.at("osVersion").get_to(osVersion);
    }
};