
/////////////////////////////////////////////////////////////////////////////////
// @file            gps_manager.cpp
// @brief           Implementation for the gps manager
// @author          Chip Brommer
/////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////
//
// Includes:
//          name                            reason included
//          ------------------              ------------------------
#include    "gps_manager.h"                 // Header
// 
/////////////////////////////////////////////////////////////////////////////////

GpsManager::GpsManager(LogClient& logger) : m_gps(GpsOptions::Unknown), m_name("GPS MGR"),
    m_configured(false), m_logger(logger), m_port(""), m_baudrate(0) 
{
    m_logger.AddLog(m_name, LogClient::LogLevel::INFO, "Initialized.");
}

GpsManager::GpsManager(LogClient& logger, const GpsOptions gps, const std::string port, const double baudrate) :
    GpsManager(logger)
{
    Configure(gps, port, baudrate);
}

GpsManager::~GpsManager()
{
    Stop();
}

void GpsManager::Configure(const GpsOptions gps, const std::string port, const double baudrate)
{
    m_logger.AddLog(m_name, LogClient::LogLevel::INFO, "Configuring for " + GpsOptionsMap.at(gps));
    m_gps = gps;
    m_port = port;
    m_baudrate = baudrate;
    m_logger.AddLog(m_name, LogClient::LogLevel::INFO, "Configured");
}

void GpsManager::Start()
{

}

void GpsManager::Stop()
{

}