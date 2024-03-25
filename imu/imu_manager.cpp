
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

ImuManager::ImuManager(LogClient& logger) : m_imu(ImuOptions::Unknown), m_name("IMU MGR"), 
    m_configured(false), m_logger(logger), m_port(""), m_baudrate(0)
{
    m_logger.AddLog(m_name, LogClient::LogLevel::INFO, "Initialized.");
}

ImuManager::ImuManager(LogClient& logger, const ImuOptions imu, const std::string port, const double baudrate) :
    ImuManager(logger)
{
    Configure(imu, port, baudrate);
}

ImuManager::~ImuManager()
{
    Stop();
}

void ImuManager::Configure(const ImuOptions imu, const std::string port, const double baudrate)
{
    m_logger.AddLog(m_name, LogClient::LogLevel::INFO, "Configuring for " + ImuOptionsMap.at(imu));
    m_imu = imu;
    m_port = port;
    m_baudrate = baudrate;
    m_logger.AddLog(m_name, LogClient::LogLevel::INFO, "Configured");
}

void ImuManager::Start()
{

}

void ImuManager::Stop()
{

}