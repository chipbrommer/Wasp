#pragma once
/////////////////////////////////////////////////////////////////////////////////
// @file            inertial_labs.h
// @brief           A class for communicating with Inertial Labs IMU modules
// @author          Chip Brommer
/////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////
//
// Includes:
//          name                            reason included
//          ------------------              ------------------------
#include <string>                           // strings
//
#include "imu_type.h"                       // base class
// 
/////////////////////////////////////////////////////////////////////////////////

/// @brief 
struct InertialLabsData
{
    double roll; 
    double pitch;
    double yaw;
    bool error;
};

/// @brief 
class InertialLabs : public ImuType
{
public:

    /// @brief 
    InertialLabs(LogClient& logger, const std::string path, const double baudrate) : 
        ImuType("ILABS", logger, path, baudrate) {}

    /// @brief 
    ~InertialLabs() {}
    
    /// @brief 
    /// @return -1 on error, else number of bytes read and processed
    int ProcessData() override;

protected:

private:

    /// @brief 
    void UpdateCommonData() override;

    InertialLabsData m_data = {};       /// Data storage
};