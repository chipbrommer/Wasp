#pragma once
/////////////////////////////////////////////////////////////////////////////////
// @file            atacnav.h
// @brief           A class for communicating with ATACNAV GPS modules
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
struct AtacnavData
{
    double roll;
    double pitch;
    double yaw;
    bool error;
};

/// @brief 
class AtacnavGps : public GpsType
{
public:

    /// @brief 
    AtacnavGps(LogClient& logger, const std::string path, const SerialClient::BaudRate baudrate) :
        GpsType("UBLOX", logger, path, baudrate) {}

    /// @brief 
    ~AtacnavGps() {}

    /// @brief 
    int ProcessData() override;

    /// @brief 
    /// @return 
    GpsData GetCommonData() override;

protected:

private:

    /// @brief 
    void UpdateCommonData() override;

    AtacnavData m_data = {};              /// Data storage
};