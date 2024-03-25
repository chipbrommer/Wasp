/////////////////////////////////////////////////////////////////////////////////
// @file            imu_manager.h
// @brief           A manager for the various IMUs 
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
#include "../utilities/log_client.h"        // logger
// 
/////////////////////////////////////////////////////////////////////////////////

class ImuManager
{
public:
    /// @brief enum indicating available imu options
    enum class ImuOptions
    {
        IL_Kernel210,
        IL_Kernel110,
    };

    /// @brief Default constructor
    ImuManager(LogClient& logger, ImuOptions imu);

    /// @brief Default deconstructor
    ~ImuManager();

    /// @brief Start the connection to the IMU
    void Start();

    /// @brief Stop the main working loop
    void Stop();

protected:

private:
    ImuOptions m_imu;                       /// Desired IMU for usage
    std::string m_name;                     /// Name for logging
    LogClient& m_logger;                    /// Logger
};