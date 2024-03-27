#pragma once
/////////////////////////////////////////////////////////////////////////////////
// @file            gps_manager.h
// @brief           A manager for controlling GPS for wasp
// @author          Chip Brommer
/////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////
//
// Includes:
//          name                            reason included
//          ------------------              ------------------------
#include <string>                           // strings
#include <unordered_map>                    // unordered map
//
#include "../utilities/log_client.h"        // logger
#include "../files/constants.h"             // Auto discovery timeout 
#include "ublox.h"                          // ublox gps
#include "novatel.h"                        // novatel gps
// 
/////////////////////////////////////////////////////////////////////////////////

class GpsManager
{
public:
    /// @brief enum indicating available gps options
    enum class GpsOptions
    {
        Unknown,
        Ublox,
        Novatel,
    };

    /// @brief Default constructor
    GpsManager(LogClient& logger);

    /// @brief Full constructor
    GpsManager(LogClient& logger, const GpsOptions option, const std::string port, const SerialClient::BaudRate baudrate);

    /// @brief Default deconstructor
    ~GpsManager();

    /// @brief Configure the serial port for the desired GPS unit
    /// @param option - desired GPS to be used
    /// @param port - in - port to connect to for communications
    /// @param baudrate - in - baudrate for the connection
    /// @return - true if successful, false if already configured and connection is opened. 
    bool Configure(const GpsOptions option, const std::string port, const SerialClient::BaudRate baudrate);

    bool AutoConfigure();

    /// @brief Start the connection to the GPS
    void Start();

    /// @brief Stop the main working loop
    void Stop();

protected:

private:

    // enum to string conversion for convenience mapping
    std::unordered_map<GpsOptions, std::string> GpsOptionsMap
    {
        {GpsOptions::Unknown,   "Unknown"},
        {GpsOptions::Ublox,     "Ublox"},
        {GpsOptions::Novatel,   "Novatel"},
    };

    GpsOptions m_currentGpsType;            /// Current GPS type 
    std::string m_name;                     /// Name for logging
    bool m_configured;                      /// Flag for if the class is configured
    LogClient& m_logger;                    /// Logger
    std::string m_port;                     /// Holds the port
    SerialClient::BaudRate m_baudrate;      /// Holds the baudrate
    std::unique_ptr<GpsType> m_gps;         /// Holds a pointer to the utilizes GPS type.  
    SerialClient m_commPort;                /// Holds connection to serial port
};