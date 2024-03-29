
/////////////////////////////////////////////////////////////////////////////////
// @file            inertial_labs.cpp
// @brief           Implementation for the inertial labs Imu class
// @author          Chip Brommer
/////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////
//
// Includes:
//          name                            reason included
//          ------------------              ------------------------
#include    "inertial_labs.h"                 // Header
// 
/////////////////////////////////////////////////////////////////////////////////

int InertialLabs::ProcessData()
{
    return -1;
}

void InertialLabs::UpdateCommonData()
{
    m_commonData.pitch = m_data.pitch;
    m_commonData.yaw = m_data.yaw;
    m_commonData.hardwareError = m_data.error;
}