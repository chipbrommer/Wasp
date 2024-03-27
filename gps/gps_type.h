#pragma once
/////////////////////////////////////////////////////////////////////////////////
// @file            gps_type.h
// @brief           Base class for a GPS unit for WASP 
// @author          Chip Brommer
/////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////
//
// Includes:
//          name                            reason included
//          ------------------              ------------------------
#include <string>                           // strings
//
#include "../utilities/serial_client.h"     // serial client
#include "../utilities/log_client.h"        // log client
// 
/////////////////////////////////////////////////////////////////////////////////

/// @brief The template of common GPS data
struct GpsData
{
    double latitude         = 0.0;
    double longitude        = 0.0;
    double altitude         = 0.0;

    bool hardwareError      = false;    // @todo fix these with some proper error types ??? 
    bool softwareError      = false;    // @todo fix these with some proper error types ??? 
};

/// @brief The base class for a GPS unit for WASP
class GpsType
{
public:

    /// @brief 
    /// @param name 
    GpsType(const std::string& name, LogClient& logger) : m_name(name), m_logger(logger) 
    {
        m_logger.AddLog(m_name, LogClient::LogLevel::Info, "Initialized.");
    }
    
    /// @brief 
    virtual ~GpsType() 
    {
        m_logger.AddLog(m_name, LogClient::LogLevel::Info, "Uninitialized.");
    }

    /// @brief 
    /// @return 
    virtual void ProcessData(std::byte* data) = 0;

    /// @brief 
    /// @return 
    GpsData GetCommonData() const { return m_commonData; }

protected:

    /// @brief 
    virtual void UpdateCommonData() = 0;

    std::string m_name              = "";           /// name of the unit
    GpsData m_commonData            = {};           /// Holds common data 
    LogClient& m_logger;

    long txCount                    = 0;            /// transmit count
    long txErrorCount               = 0;            /// transmit error count
    long rxCount                    = 0;            /// receive count
    long rxErrorCount               = 0;            /// receive error count

};