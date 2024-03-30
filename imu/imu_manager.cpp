
/////////////////////////////////////////////////////////////////////////////////
// @file            imu_manager.cpp
// @brief           Implementation for the IMU manager
// @author          Chip Brommer
/////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////
//
// Includes:
//          name                            reason included
//          ------------------              ------------------------
#include <string>                           // strings
//
#include "imu_manager.h"                    // header
// 
/////////////////////////////////////////////////////////////////////////////////

ImuManager::ImuManager(LogClient& logger) : m_imuOption(ImuOptions::Unknown), m_name("IMU MGR"),
    m_configured(false), m_logger(logger)
{
    m_logger.AddLog(m_name, LogClient::LogLevel::Info, "Initialized.");
}

ImuManager::ImuManager(LogClient& logger, const ImuOptions option, const std::string port, const SerialClient::BaudRate baudrate) :
    ImuManager(logger)
{
    Configure(option, port, baudrate);
}

ImuManager::~ImuManager()
{
    Stop();
}

bool ImuManager::Configure(const ImuOptions option, const std::string port, const SerialClient::BaudRate baudrate)
{
    
    if (m_configured)
    {
        return true;
    }

    m_logger.AddLog(m_name, LogClient::LogLevel::Info, "Configuring for " + ImuOptionsMap.at(option));
    m_imuOption = option;

    // Create the proper IMU
    switch (option)
    {
    case ImuOptions::IL_Kernel210:
    case ImuOptions::IL_Kernel110:
        m_imu = std::make_unique<InertialLabs>(m_logger, port, baudrate);
        break;
    case ImuOptions::Unknown:
        // Intentionally do nothing... 
    default:
        m_logger.AddLog(m_name, LogClient::LogLevel::Error, "Unsupported IMU option selected.");
        return false;
    };

    m_logger.AddLog(m_name, LogClient::LogLevel::Info, "Configured");

    return true;
}

void ImuManager::Start()
{
    // Start the IMU in a blocking function
}

void ImuManager::Stop()
{
    // Stop the IMU
}

int ImuManager::CheckForData()
{
    return m_imu->ProcessData();
}

int ImuManager::Send(const std::byte* data, const size_t length)
{
    return m_commPort.Write(data, length);
}