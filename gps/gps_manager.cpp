
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

GpsManager::GpsManager(LogClient& logger) : m_gpsOption(GpsOptions::Unknown), m_name("GPS MGR"),
    m_configured(false), m_logger(logger), m_port(""), m_baudrate(0) 
{
    m_logger.AddLog(m_name, LogClient::LogLevel::Info, "Initialized.");
}

GpsManager::GpsManager(LogClient& logger, const GpsOptions option, const std::string port, const double baudrate) :
    GpsManager(logger)
{
    Configure(option, port, baudrate);
}

GpsManager::~GpsManager()
{
    Stop();
}

bool GpsManager::Configure(const GpsOptions option, const std::string port, const double baudrate)
{
    m_logger.AddLog(m_name, LogClient::LogLevel::Info, "Configuring for " + GpsOptionsMap.at(option));
    m_gpsOption = option;
    m_port = port;
    m_baudrate = baudrate;

    switch (option)
    {
    case GpsOptions::Ublox:
        m_gps = std::make_unique<Ublox>();
        break;
    case GpsOptions::Unknown:
        // Intentionally do nothing... 
    default:
        m_logger.AddLog(m_name, LogClient::LogLevel::Error, "Unsupported GPS option selected.");
        return false;
    };

    m_logger.AddLog(m_name, LogClient::LogLevel::Info, "Configured");
}

void GpsManager::Start()
{
    // @todo
}

void GpsManager::Stop()
{
    // @todo
}