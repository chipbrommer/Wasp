
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

GpsManager::GpsManager(LogClient& logger) : m_currentGpsType(GpsOptions::Unknown), m_name("GPS MGR"),
    m_configured(false), m_logger(logger), m_port(""), m_baudrate(SerialClient::BaudRate::BAUDRATE_INVALID)
{
    m_logger.AddLog(m_name, LogClient::LogLevel::Info, "Initialized.");
}

GpsManager::GpsManager(LogClient& logger, const GpsOptions option, const std::string port, const SerialClient::BaudRate baudrate) :
    GpsManager(logger)
{
    Configure(option, port, baudrate);
}

GpsManager::~GpsManager()
{
    Stop();
}

bool GpsManager::Configure(const GpsOptions option, const std::string port, const SerialClient::BaudRate baudrate)
{
    bool rtn = false;

    // If the requested option is the same as the current option, return true
    if (m_gps && option == m_currentGpsType)
    {
        m_logger.AddLog(m_name, LogClient::LogLevel::Error, "GPS already configured with the same option.");
        return true; // Already configured with the same option, so return true
    }

    m_logger.AddLog(m_name, LogClient::LogLevel::Info, "Configuring for " + GpsOptionsMap.at(option));
    m_currentGpsType = option;
    m_port = port;
    m_baudrate = baudrate;

    switch (option)
    {
    case GpsOptions::Ublox:
        m_gps = std::make_unique<Ublox>(m_logger, port, baudrate);
        break;
    case GpsOptions::Novatel:
        m_gps = std::make_unique<Novatel>(m_logger, port, baudrate);
        break;
    case GpsOptions::Unknown:
        // Intentional fall through
    default:
        m_logger.AddLog(m_name, LogClient::LogLevel::Error, "Unsupported GPS option selected.");
        m_currentGpsType = GpsOptions::Unknown;
        m_gps = nullptr;
    };

    if (baudrate == SerialClient::BaudRate::BAUDRATE_AUTO)
    {
        SerialClient::BaudRate baud = m_gps->AutoDiscoverBaudRate();

        // if return was invalid, we failed to create connection and initialize. 
        if (baud != SerialClient::BaudRate::BAUDRATE_INVALID) rtn = true;
    }
    else
    {
        // If the baudrate isnt invalid, it indicates a successful connection
        if (m_gps->GetBaudRate() != SerialClient::BaudRate::BAUDRATE_INVALID) rtn = true;
    }

    if (rtn) { m_logger.AddLog(m_name, LogClient::LogLevel::Info, "Configured"); }
    else { m_logger.AddLog(m_name, LogClient::LogLevel::Info, "Failed to configure"); }
    return rtn;
}

bool GpsManager::AutoConfigure()
{
    // List of GPS options to iterate through
    std::vector<GpsOptions> gpsOptionsList = { GpsOptions::Ublox, GpsOptions::Novatel };

    // Iterate through GPS options
    for (const auto& option : gpsOptionsList)
    {
        m_logger.AddLog(m_name, LogClient::LogLevel::Info, "Attempting to auto-configure for " + GpsOptionsMap.at(option));

        // Configure the m_gps pointer
        if (!Configure(option, m_port, m_baudrate))
        {
            m_logger.AddLog(m_name, LogClient::LogLevel::Error, "Failed to configure GPS option: " + GpsOptionsMap.at(option));
            continue; // Try next GPS option
        }

        // Wait for specified timeout seconds to attempt to get data
        auto startTime = std::chrono::steady_clock::now();
        while (std::chrono::steady_clock::now() - startTime < std::chrono::seconds(AUTO_DISCOVERY_TIMEOUT_SECS))
        {
            // Process data continuously
            m_gps->ProcessData();

            // Check if data is received
            if (m_gps->GetCommonData().rxCount > 0)
            {
                m_logger.AddLog(m_name, LogClient::LogLevel::Info, "Auto-configuration successful for " + GpsOptionsMap.at(option));
                return true; // Data received, configuration successful
            }

            // Sleep for a short duration before processing data again
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }

        // If data is not received within 30 seconds
        m_logger.AddLog(m_name, LogClient::LogLevel::Error, "Failed to receive data for " + GpsOptionsMap.at(option) + ". Retrying with next option...");
    }

    // If no GPS option succeeded in receiving data
    m_logger.AddLog(m_name, LogClient::LogLevel::Error, "Auto-configuration failed. Unable to establish communication with any GPS option.");
    return false;
}


void GpsManager::Start()
{
    // @todo
}

void GpsManager::Stop()
{
    // @todo
}