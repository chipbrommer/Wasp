#pragma once
/////////////////////////////////////////////////////////////////////////////////
// @file            gps_manager.h
// @brief           A manager for controlling gps for wasp
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
#include "../utilities/log_client.h"           // logger
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
    };

    /// @brief Default constructor
    GpsManager(LogClient& logger);

    /// @brief Full constructor
    GpsManager(LogClient& logger, const GpsOptions option, const std::string port, const double baudrate);

    /// @brief Default deconstructor
    ~GpsManager();

    /// @brief Configure the serial port for the desired GPS unit
    /// @param imu - desired GPS to be used
    /// @param port - in - port to connect to for communications
    /// @param baudrate - in - baudrate for the connection
    void Configure(const GpsOptions imu, const std::string port, const double baudrate);

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
        {GpsOptions::Ublox,     "Ublox"}
    };

    GpsOptions m_gps;                       /// Desired GPS unit
    std::string m_name;                     /// Name for logging
    bool m_configured;                      /// Flag for if the class is configured
    LogClient& m_logger;                    /// Logger
    std::string m_port;                     /// Holds the port
    double m_baudrate;                      /// Holds the baudrate
};