/////////////////////////////////////////////////////////////////////////////////
// @file            configuration.h
// @brief           Structure for the device configuration information
// @author          Chip Brommer
/////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////
//
// Includes:
//          name                            reason included
//          ------------------              ------------------------
#include <string>                           // strings
//
#include "../external/nlohmann/json.hpp"       // json
#include "../utilities/json_file_utility.hpp"  // file utility type
#include "../gps/gps_manager.h"
#include "../imu/imu_manager.h"
//
/////////////////////////////////////////////////////////////////////////////////

/// @brief A structure representing a configration file
struct Configuration : JsonType
{
    // Ports
    std::string imuPort     = "";
    SerialClient::BaudRate imuBaudRate  = SerialClient::BaudRate::BAUDRATE_115200;
    std::string gpsPort     = "";
    SerialClient::BaudRate gpsBaudRate  = SerialClient::BaudRate::BAUDRATE_38400;

    // PWMs / Fins
    std::string fin1Path    = "";
    int fin1Channel         = -1;
    std::string fin2Path    = "";
    int fin2Channel         = -1;
    std::string fin3Path    = "";
    int fin3Channel         = -1;
    std::string fin4Path    = "";
    int fin4Channel         = -1;
    double finMinDegrees    = -25.0;
    double finMaxDegrees    = 25.0;

    // Hardware Selection
    GpsManager::GpsOptions gpsUnit       = GpsManager::GpsOptions::Ublox;
    ImuManager::ImuOptions imuUnit       = ImuManager::ImuOptions::IL_Kernel210;

    /// @brief map for json item to variables
    std::unordered_map<std::string, std::function<void(const nlohmann::json&)>> jsonMapping
    {
        {"imuPort",             [this](const nlohmann::json& j) { j.at("imuPort").get_to(imuPort);                          }},
        {"imuBaudRate",         [this](const nlohmann::json& j) { j.at("imuBaudRate").get_to(imuBaudRate);                  }},
        {"gpsPort",             [this](const nlohmann::json& j) { j.at("gpsPort").get_to(gpsPort);                          }},
        {"gpsBaudRate",         [this](const nlohmann::json& j) { j.at("gpsBaudRate").get_to(gpsBaudRate);                  }},
        {"fin1Path",            [this](const nlohmann::json& j) { j.at("fin1Path").get_to(fin1Path);                        }},
        {"fin1Channel",         [this](const nlohmann::json& j) { j.at("fin1Channel").get_to(fin1Channel);                  }},
        {"fin2Path",            [this](const nlohmann::json& j) { j.at("fin2Path").get_to(fin2Path);                        }},
        {"fin2Channel",         [this](const nlohmann::json& j) { j.at("fin2Channel").get_to(fin2Channel);                  }},
        {"fin3Path",            [this](const nlohmann::json& j) { j.at("fin3Path").get_to(fin3Path);                        }},
        {"fin3Channel",         [this](const nlohmann::json& j) { j.at("fin3Channel").get_to(fin3Channel);                  }},
        {"fin4Path",            [this](const nlohmann::json& j) { j.at("fin4Path").get_to(fin4Path);                        }},
        {"fin4Channel",         [this](const nlohmann::json& j) { j.at("fin4Channel").get_to(fin4Channel);                  }},
        {"finMinDegrees",       [this](const nlohmann::json& j) { j.at("finMinDegrees").get_to(finMinDegrees);              }},
        {"finMaxDegrees",       [this](const nlohmann::json& j) { j.at("finMaxDegrees").get_to(finMaxDegrees);              }},
        {"gpsUnit",             [this](const nlohmann::json& j) { j.at("gpsUnit").get_to(gpsUnit);                          }},
        {"imuUnit",             [this](const nlohmann::json& j) { j.at("imuUnit").get_to(imuUnit);                          }}
    };

    /// @brief Serialize structure to json
    /// @return json structure containing structure data
    nlohmann::json ToJson() const
    {
        return nlohmann::json{
            {"imuPort",             imuPort},
            {"imuBaudRate",         imuBaudRate},
            {"gpsPort",             gpsPort},
            {"gpsBaudRate",         gpsBaudRate},
            {"fin1Path",            fin1Path},
            {"fin1Channel",         fin1Channel},
            {"fin2Path",            fin2Path},
            {"fin2Channel",         fin2Channel},
            {"fin3Path",            fin3Path},
            {"fin3Channel",         fin3Channel},
            {"fin4Path",            fin4Path},
            {"fin4Channel",         fin4Channel},
            {"finMinDegrees",       finMinDegrees},
            {"finMaxDegrees",       finMaxDegrees},
            {"gpsUnit",             gpsUnit},
            {"imuUnit",             imuUnit}
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
            if (j.contains(key)) { func(j); }
        }
    }
};