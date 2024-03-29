#pragma once
/////////////////////////////////////////////////////////////////////////////////
// @file            ublox.h
// @brief           A class for communicating with Ublox GPS modules
// @author          Chip Brommer
/////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////
//
// Includes:
//          name                            reason included
//          ------------------              ------------------------
#include <string>                           // strings
//
#include "gps_type.h"                       // base class
// 
/////////////////////////////////////////////////////////////////////////////////

/// @brief 
struct UbloxData
{
    double roll;
    double pitch;
    double yaw;
    bool error;
};

/// @brief 
class Ublox : public GpsType
{
public:

    /// @brief 
    Ublox(LogClient& logger, const std::string path, const SerialClient::BaudRate baudrate);

    /// @brief 
    ~Ublox() {}

    /// @brief 
    int ProcessData() override;

protected:

private:

    /// @brief 
    void UpdateCommonData() override;

    UbloxData m_data = {};              /// Data storage
};