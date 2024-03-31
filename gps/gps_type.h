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

        // Open the port and configure it to 9600 (lowest baud)
        m_comms.OpenConfigure(m_path, SerialClient::BaudRate::BAUDRATE_9600, SerialClient::ByteSize::EIGHT, SerialClient::Parity::NONE, SerialClient::StopBits::ONE);

        // Iterate over the baud rate map
        for (const auto& [baudRateEnum, baudRateValue] : m_GpsCommonBaudRateMap)
        {
            m_logger.AddLog(m_name, LogClient::LogLevel::Info, "Trying baud rate: " + std::to_string(baudRateValue));

            if (baudRateEnum != m_comms.GetBaudRate())
            {
                // Try to open the port with the current baud rate
                if (!m_comms.Reconfigure(m_path, baudRateEnum, SerialClient::ByteSize::EIGHT, SerialClient::Parity::NONE, SerialClient::StopBits::ONE))
                {
                    m_logger.AddLog(m_name, LogClient::LogLevel::Info, "Auto baud discovery failed to configure port.");
                    return false;
                }
            }

            // Wait for set timeout length to attempt to get data
            auto startTime = std::chrono::steady_clock::now();
            while (std::chrono::steady_clock::now() - startTime < std::chrono::seconds(AUTO_DISCOVERY_TIMEOUT_SECS))
            {
                // Attempt to get and process data
                //ProcessData();

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

    /// @brief Necessary function to read and process data that all GPS units must use
    /// @return -1 on fail, else 0+
    virtual int ProcessData() = 0;

    /// @brief Get a copy of the common GPS daa
    /// @return GpsData copy
    GpsData GetCommonData() const { return m_commonData; }

    /// @brief Check if the GPS unit was initialized correctly 
    /// @returntrue for successfully initalized, else false 
    bool Initialized() const { return m_initialized; }

protected:

    /// @brief Necessary function to update common data that all GPS units must provide
    virtual void UpdateCommonData() = 0;

    /// @brief mapping for common baud rates. Used in AutoDiscoverBaudRate()
    std::unordered_map<SerialClient::BaudRate, int> m_GpsCommonBaudRateMap = {
            {SerialClient::BaudRate::BAUDRATE_9600, 9600},
            {SerialClient::BaudRate::BAUDRATE_19200, 19200},
            {SerialClient::BaudRate::BAUDRATE_38400, 38400},
            {SerialClient::BaudRate::BAUDRATE_115200, 115200},
            {SerialClient::BaudRate::BAUDRATE_921600, 921600}
    };

    std::string             m_name              = "";           /// name of the unit
    GpsData                 m_commonData        = {};           /// Holds common data 
    LogClient&              m_logger;                           /// Holds the logger instance
    std::string             m_path              = "";           /// Holds the path to desired serial port
    SerialClient::BaudRate  m_baudrate          = SerialClient::BaudRate::BAUDRATE_INVALID;     /// Holds the baudrate
    SerialClient            m_comms;                            /// Holds the serial client
    bool                    m_initialized       = false;        /// Bool to hold if the gps unit is initialized correctly
};