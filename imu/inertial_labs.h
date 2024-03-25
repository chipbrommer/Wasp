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
    InertialLabs() : ImuType("") {}

    /// @brief 
    ~InertialLabs() {}
    
    /// @brief 
    /// @return 
    int ReadData() override;

    /// @brief 
    /// @return 
    int SendData() override;

protected:

private:

    /// @brief 
    void ProcessData();

    /// @brief 
    void UpdateCommonData() override;

    InertialLabsData m_data = {};       /// Data storage
};