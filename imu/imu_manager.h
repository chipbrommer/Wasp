#pragma once
/////////////////////////////////////////////////////////////////////////////////
// @file            imu_manager.h
// @brief           A manager for the various IMUs 
// @author          Chip Brommer
/////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////
//
// Includes:
//          name                            reason included
//          ------------------              ------------------------
#include <string>                           // strings
#include <unordered_map>                    // unordered map
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
        Unknown,
        IL_Kernel210,
        IL_Kernel110,
    };

    /// @brief Default constructor
    ImuManager(LogClient& logger);

    /// @brief Full constructor
    ImuManager(LogClient& logger, const ImuOptions option, const std::string port, const double baudrate);

    /// @brief Default deconstructor
    ~ImuManager();

    /// @brief Configure the serial port for the desired IMU
    /// @param imu - desired IMU to be used
    /// @param port - in - port to connect to for communications
    /// @param baudrate - in - baudrate for the connection
    void Configure(const ImuOptions imu, const std::string port, const double baudrate);

    /// @brief Start the connection to the IMU
    void Start();

    /// @brief Stop the connection to the IMU
    void Stop();

protected:

private:

    // enum to string conversion for convenience mapping
    std::unordered_map<ImuOptions, std::string> ImuOptionsMap
    {
        {ImuOptions::Unknown, "Unknown"},
        {ImuOptions::IL_Kernel210, "Inertial Labs Kernel-210"},
        {ImuOptions::IL_Kernel110, "Inertial Labs Kernel-110"},
    };

    ImuOptions m_imu;                       /// Desired IMU for usage
    std::string m_name;                     /// Name for logging
    bool m_configured;                      /// Flag for if the class is configured
    LogClient& m_logger;                    /// Logger
    std::string m_port;                     /// Holds the port
    double m_baudrate;                      /// Holds the baudrate
};