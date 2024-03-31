#pragma once
/////////////////////////////////////////////////////////////////////////////////
// @file            gps_type.h
// @brief           Base class for a GPS unit for WASP 
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
#include "../utilities/log_client.h"        // log client
#include "../files/constants.h"             // Auto discovery timeout 
// 
/////////////////////////////////////////////////////////////////////////////////

/// @brief The template of common GPS data
struct GpsData
{
    double  latitude        = 0.0;
    double  longitude       = 0.0;
    double  altitude        = 0.0;

    bool    hardwareError   = false;    // @todo fix these with some proper error types ??? 
    bool    softwareError   = false;    // @todo fix these with some proper error types ??? 

    long    txCount         = 0;        // transmit count
    long    txErrorCount    = 0;        // transmit error count
    long    rxCount         = 0;        // receive count
    long    rxErrorCount    = 0;        // receive error count
};

/// @brief The base class for a GPS unit for WASP
class GpsType
{
public:

    /// @brief 
    /// @param name 
    /// @param logger 
    /// @param path 
    /// @param baudrate 
    GpsType(const std::string& name, LogClient& logger, const std::string path, const SerialClient::BaudRate baudrate) :
        m_name(name), m_logger(logger), m_path(path), m_baudrate(baudrate)
    {
        bool success = false;

        // Attempt to auto discover if we received auto
        if (m_baudrate != SerialClient::BaudRate::BAUDRATE_AUTO && m_baudrate != SerialClient::BaudRate::BAUDRATE_INVALID)
        {
            success = m_comms.OpenConfigure(m_path, m_baudrate, SerialClient::ByteSize::EIGHT, SerialClient::Parity::NONE, SerialClient::StopBits::ONE);
        }
        else if (m_baudrate == SerialClient::BaudRate::BAUDRATE_AUTO)
        {
            success = AutoDiscoverBaudRate();
        }

        if(success) m_logger.AddLog(m_name, LogClient::LogLevel::Info, "Initialized.");
        else m_logger.AddLog(m_name, LogClient::LogLevel::Error, "Initialized Failed");
    }
    
    /// @brief Default base deconstructor
    virtual ~GpsType() 
    {
        m_comms.Close();
        m_logger.AddLog(m_name, LogClient::LogLevel::Info, "Uninitialized.");
    }

    /// @brief Attempts to connect to the GPS unit without a specific baudrate
    /// @return true if successful connection established, else false
    bool AutoDiscoverBaudRate()
    {
        m_logger.AddLog(m_name, LogClient::LogLevel::Info, "Auto baud discovery enabled.");

        // Iterate over the baud rate map
        for (const auto& [baudRateEnum, baudRateValue] : m_GpsCommonBaudRateMap)
        {
            m_logger.AddLog(m_name, LogClient::LogLevel::Info, "Trying baud rate: " + std::to_string(baudRateValue));

            // Try to open the port with the current baud rate
            if (!m_comms.OpenConfigure(m_path, baudRateEnum, SerialClient::ByteSize::EIGHT, SerialClient::Parity::NONE, SerialClient::StopBits::ONE))
            {
                m_logger.AddLog(m_name, LogClient::LogLevel::Info, "Auto baud discovery failed to configure port.");
                return false;
            }

            // Wait for set timeout length to attempt to get data
            auto startTime = std::chrono::steady_clock::now();
            while (std::chrono::steady_clock::now() - startTime < std::chrono::seconds(AUTO_DISCOVERY_TIMEOUT_SECS))
            {
                // Attempt to get and process data
                ProcessData();

                // Check if data is received
                if (m_commonData.rxCount > 0)
                {
                    m_logger.AddLog(m_name, LogClient::LogLevel::Info, "Baudrate successful for " + std::to_string(baudRateValue));
                    m_baudrate = baudRateEnum;
                    return true;
                }

                // Sleep for a short duration before processing data again
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
            }
        }

        m_logger.AddLog(m_name, LogClient::LogLevel::Error, "Failed to auto-discover baud rate.");
        return false; // Failed to discover baud rate
    }

    /// @brief Get the current baudrate
    /// @return - Configured baudrate, SerialClient::BaudRate::BAUDRATE_INVALID indicates connection not opened. 
    SerialClient::BaudRate GetBaudRate() const { return m_baudrate; }

    /// @brief 
    /// @return 
    virtual int ProcessData() = 0;

    /// @brief 
    /// @return 
    GpsData GetCommonData() const { return m_commonData; }

protected:

    /// @brief 
    virtual void UpdateCommonData() = 0;

    std::unordered_map<SerialClient::BaudRate, int> m_GpsCommonBaudRateMap = {
            {SerialClient::BaudRate::BAUDRATE_9600, 9600},
            {SerialClient::BaudRate::BAUDRATE_19200, 19200},
            {SerialClient::BaudRate::BAUDRATE_38400, 38400},
            {SerialClient::BaudRate::BAUDRATE_115200, 115200},
            {SerialClient::BaudRate::BAUDRATE_921600, 921600}
    };

    std::string             m_name              = "";           /// name of the unit
    GpsData                 m_commonData        = {};           /// Holds common data 
    LogClient&              m_logger;
    std::string             m_path              = "";           /// Holds the path to desired serial port
    SerialClient::BaudRate  m_baudrate          = SerialClient::BaudRate::BAUDRATE_INVALID;     /// Holds the baudrate
    SerialClient            m_comms;                            /// Holds the serial client
};