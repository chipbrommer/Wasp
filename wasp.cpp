/////////////////////////////////////////////////////////////////////////////////
// @file            wasp_m.cpp
// @brief           Wasp implementation
// @author          Chip Brommer
/////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////
//
// Includes:
//          name                            reason included
//          ------------------              ------------------------
#include "wasp.h"                           // Header
// 
/////////////////////////////////////////////////////////////////////////////////

Wasp::Wasp(const std::string& settingsLocation, const std::string& buildLocation, const std::string& configLocation) :
    m_settings(settingsLocation), m_build(buildLocation), m_config(configLocation),
    m_name("WASP"), m_logger(), m_signalManger(m_logger), m_imuManager(m_logger),
    m_gpsManager(m_logger)
{
    // Load the configs and catch any failures
    if (!m_settings.Load())
    {
        std::cerr << "[WASP] - ERROR - Failed to Load/Create settings file.\n";
    }

    if (!m_build.Load())
    {
        std::cerr << "[WASP] - ERROR - Failed to Load/Create build file.\n";
    }

    if (!m_config.Load())
    {
        std::cerr << "[WASP] - ERROR - Failed to Load/Create config file.\n";
    }

    // Start the logger, if file logging is enabled in config, enable it. 
    if (m_settings.data.fileLoggingEnabled && !m_settings.data.logFilePath.empty())
    {
        m_logger.EnableFileLogging(m_settings.data.logFilePath);
    }
    m_loggingThread = std::thread([this] { m_logger.Run(); });

    // Initialize the signal manager PWMs
    m_logger.AddLog(m_name, LogClient::LogLevel::Info, "Starting Signal Manager.");
    m_signalManger.ReadyFin(SignalManager::FIN::ONE,      m_config.data.fin1Path, m_config.data.fin1Channel, m_config.data.finMinDegrees, m_config.data.finMaxDegrees);
    m_signalManger.ReadyFin(SignalManager::FIN::TWO,      m_config.data.fin2Path, m_config.data.fin2Channel, m_config.data.finMinDegrees, m_config.data.finMaxDegrees);
    m_signalManger.ReadyFin(SignalManager::FIN::THREE,    m_config.data.fin3Path, m_config.data.fin3Channel, m_config.data.finMinDegrees, m_config.data.finMaxDegrees);
    m_signalManger.ReadyFin(SignalManager::FIN::FOUR,     m_config.data.fin4Path, m_config.data.fin4Channel, m_config.data.finMinDegrees, m_config.data.finMaxDegrees);
    m_signalThread = std::thread([this] { m_signalManger.Start(); });

    // Configure the IMU
    if(!m_imuManager.Configure(m_config.data.imuUnit, m_config.data.imuPort, m_config.data.imuBaudRate))
    {
        m_logger.AddLog(m_name, LogClient::LogLevel::Info, "IMU Manager failed to configure, exiting.");
        Close();
    }

    // Configure the GPS
    if (!m_gpsManager.Configure(m_config.data.gpsUnit, m_config.data.gpsPort, SerialClient::BaudRate::BAUDRATE_AUTO))
    {
        m_logger.AddLog(m_name, LogClient::LogLevel::Info, "GPS Manager failed to configure, exiting.");
        Close();
    }
}

Wasp::~Wasp() 
{
    Close();
}

void Wasp::Execute()
{
    while (true)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
}

void Wasp::Close()
{
    m_signalManger.Stop();
    if (m_signalThread.joinable()) m_signalThread.join();

    // Close the logger last but wait until all logs have been written
    m_logger.Stop(true);
    if (m_loggingThread.joinable()) m_loggingThread.join();
}