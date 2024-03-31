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
#include "inertial_labs.h"                  // inertial labs 
#include "../utilities/constants.h"         // constants
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
    ImuManager(LogClient& logger, const ImuOptions option, const std::string port, const SerialClient::BaudRate baudrate);

    /// @brief Default deconstructor
    ~ImuManager();

    /// @brief Configure the serial port for the desired IMU
    /// @param option - desired IMU to be used
    /// @param port - in - port to connect to for communications
    /// @param baudrate - in - baudrate for the connection
    /// @return - true if successful, false if already configured and connection is opened. 
    bool Configure(const ImuOptions option, const std::string port, const SerialClient::BaudRate baudrate);

    /// @brief Start the connection to the IMU
    void Start();

    /// @brief Stop the connection to the IMU
    void Stop();

    /// @brief Read and Process data from the configured IMU
    /// @return -1 on error, else number of bytes read
    int CheckForData();

    /// @brief Send data for the configured IMU
    /// @param data - in - reference to the data to be sent
    /// @param length - in - number of bytes to send
    /// @return -1 on error, else number of bytes sent
    int Send(const std::byte* data, const size_t length);

protected:

private:

    // enum to string conversion for convenience mapping
    std::unordered_map<ImuOptions, std::string> ImuOptionsMap
    {
        {ImuOptions::Unknown,           "Unknown"},
        {ImuOptions::IL_Kernel210,      "Inertial Labs Kernel-210"},
        {ImuOptions::IL_Kernel110,      "Inertial Labs Kernel-110"},
    };

    ImuOptions                  m_currentImuType;   /// Desired IMU for usage
    std::string                 m_name;             /// Name for logging
    bool                        m_configured;       /// Flag for if the class is configured
    LogClient&                  m_logger;           /// Logger
    std::string                 m_port;             /// Holds the port
    SerialClient::BaudRate      m_baudrate;         /// Holds the baudrate
    std::unique_ptr<ImuType>    m_imu;              /// Holds a pointer to the utilized IMU type. 
    SerialClient                m_commPort;         /// Holds connection to serial port
    std::atomic_bool            m_run;              /// Holds an bool to kill the main loop

};