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
// 
/////////////////////////////////////////////////////////////////////////////////

/// @brief The template of common GPS data
struct GpsData
{
    double latitude = 0.0;
    double longitude = 0.0;
    double altitude = 0.0;

    bool hardwareError = false; // @todo fix these with some proper error types ??? 
    bool softwareError = false; // @todo fix these with some proper error types ??? 

    long txCount = 0;
    long txErrorCount = 0;
    long rxCount = 0;
    long rxErrorCount = 0;
};

/// @brief The base class for a GPS unit for WASP
class GpsType
{
public:

    /// @brief 
    /// @param name 
    GpsType(const std::string& name) : m_name(name) {}
    
    /// @brief 
    ~GpsType() {}

    /// @brief 
    /// @return 
    virtual bool Connect() = 0;

    /// @brief 
    virtual void Disconnect() = 0;

    /// @brief 
    /// @return 
    virtual int ReadData() = 0;

    /// @brief 
    /// @return 
    virtual bool ProcessData() = 0;

    /// @brief 
    /// @return 
    virtual bool SendData() = 0;

    /// @brief 
    /// @return 
    GpsData GetCommonData() const { return m_commonData; }

protected:

    /// @brief 
    virtual void UpdateCommonData() = 0;

    std::string m_name;                     /// name of the unit
    GpsData m_commonData;                   /// Holds common data 

private:

};