/////////////////////////////////////////////////////////////////////////////////
// @file            signal_manager.h
// @brief           A manager for controlling signals for Wasp
// @author          Chip Brommer
/////////////////////////////////////////////////////////////////////////////////
#pragma once
/////////////////////////////////////////////////////////////////////////////////
//
// Includes:
//          name                            reason included
//          ------------------              ------------------------
#include <string>                           // strings
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
        Ublox_M8,
        Ublox_M9,
        Ublox_M10,
    };

    /// @brief Default constructor
    GpsManager(LogClient& logger, GpsOptions gps);

    /// @brief Constructor
    GpsManager();

    /// @brief Default deconstructor
    ~GpsManager();

    /// @brief Start the connection to the GPS
    void Start();

    /// @brief Stop the main working loop
    void Stop();

protected:

private:
    GpsOptions m_gps;                       /// Desired GPS unit
    std::string m_name;                     /// Name for logging
    LogClient& m_logger;                    /// Logger
};