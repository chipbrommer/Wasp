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

/// @brief A structure representing a build log
struct Build : JsonType
{
    std::string imuSerialNumber = "";
    std::string gpsSerialNumber = "";
    std::string osVersion = "";

    /// @brief map for json item to variables
    std::unordered_map<std::string, std::function<void(const nlohmann::json&)>> jsonMapping
    {
        {"imuSerialNumber", [this](const nlohmann::json& j) { j.at("imuSerialNumber").get_to(imuSerialNumber);  }},
        {"gpsSerialNumber", [this](const nlohmann::json& j) { j.at("gpsSerialNumber").get_to(gpsSerialNumber);  }},
        {"osVersion",       [this](const nlohmann::json& j) { j.at("osVersion").get_to(osVersion);              }}
    };

    /// @brief Mandatory function for serializing settings to json
    /// @param j - out - json object containing settings
    void ToJson(nlohmann::json& j) const
    {
        j = ToJson();
    }

    /// @brief Serialize structure to json
    /// @return json structure containing structure data
    nlohmann::json ToJson() const
    {
        return nlohmann::json{
            {"imuSerialNumber", imuSerialNumber},
            {"gpsSerialNumber", gpsSerialNumber},
            {"osVersion", osVersion}
        };
    }
       
    /// @brief Mandatory function for deserializing settings from json
    /// @param j - in - json object containing settings
    void FromJson(const nlohmann::json& j)
    {
        for (const auto& [key, func] : jsonMapping)
        {
            if (j.contains(key)) { func(j); }
        }
    }
};