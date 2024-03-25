#pragma once
/////////////////////////////////////////////////////////////////////////////////
// @file            imu_type.h
// @brief           Base class for an IMU unit for WASP 
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

/// @brief The template of common IMU data
struct ImuData
{
    double roll = 0.0;
    double pitch = 0.0;
    double yaw = 0.0;
    double rollDelta = 0.0;
    double pitchDelta = 0.0;
    double yawDelta = 0.0;

    bool hardwareError = false; // @todo fix these with some proper error types ??? 
    bool softwareError = false; // @todo fix these with some proper error types ??? 

};

/// @brief The base class for an IMU unit for WASP
class ImuType
{
public:

    /// @brief 
    /// @param name 
    ImuType(const std::string& name) : m_name(name) {}

    /// @brief 
    ~ImuType() {}

    /// @brief 
    /// @return 
    virtual int ReadData() = 0;

    /// @brief 
    /// @return 
    virtual int SendData() = 0;

    /// @brief 
    /// @return 
    ImuData GetCommonData() const { return m_commonData; }

protected:
    /// @brief 
    virtual void UpdateCommonData() = 0;

    std::string m_name;
    ImuData m_commonData;

    long m_txCount = 0;
    long m_txErrorCount = 0;
    long m_rxCount = 0;
    long m_rxErrorCount = 0;

private:

};