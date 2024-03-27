#pragma once
/////////////////////////////////////////////////////////////////////////////////
// @file            novatel.h
// @brief           A class for communicating with Novatel GPS modules
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
struct NovatelData
{
    double roll;
    double pitch;
    double yaw;
    bool error;
};

/// @brief 
class Novatel : public GpsType
{
public:

    /// @brief 
    Novatel(LogClient& logger, const std::string path, const SerialClient::BaudRate baudrate) :
        GpsType("UBLOX", logger, path, baudrate) {}

    /// @brief 
    ~Novatel() {}

    /// @brief 
    int ProcessData() override;

protected:

private:

    /// @brief 
    void UpdateCommonData() override;

    NovatelData m_data = {};              /// Data storage
};