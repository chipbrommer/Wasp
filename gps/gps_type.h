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
// 
/////////////////////////////////////////////////////////////////////////////////

/// @brief The template of common GPS data
struct GpsData
{
    double latitude         = 0.0;
    double longitude        = 0.0;
    double altitude         = 0.0;

    bool hardwareError      = false;    // @todo fix these with some proper error types ??? 
    bool softwareError      = false;    // @todo fix these with some proper error types ??? 
};

/// @brief The base class for a GPS unit for WASP
class GpsType
{
public:

    /// @brief 
    /// @param name 
    GpsType(const std::string& name, LogClient& logger, const std::string path, const SerialClient::BaudRate baudrate) :
        m_name(name), m_logger(logger), m_path(path), m_baudrate(baudrate)
    {
        m_comms.OpenConfigure(m_path, m_baudrate, SerialClient::ByteSize::EIGHT, SerialClient::Parity::NONE);
        m_logger.AddLog(m_name, LogClient::LogLevel::Info, "Initialized.");
    }
    
    /// @brief 
    virtual ~GpsType() 
    {
        m_comms.Close();
        m_logger.AddLog(m_name, LogClient::LogLevel::Info, "Uninitialized.");
    }

    /// @brief 
    /// @return 
    virtual int ProcessData() = 0;

    /// @brief 
    /// @param data - in - data to be sent
    /// @param size - in - size of data to be sent
    /// @return -1 on error, else number of bytes sent
    int Send(const std::byte& data, const size_t size)
    {
        if (!m_comms.IsOpen()) return -1;
        return m_comms.Write(data, size);
    }

    /// @brief 
    /// @return 
    GpsData GetCommonData() const { return m_commonData; }

protected:

    /// @brief 
    virtual void UpdateCommonData() = 0;

    std::string             m_name              = "";           /// name of the unit
    GpsData                 m_commonData        = {};           /// Holds common data 
    LogClient&              m_logger;
    std::string             m_path              = "";           /// Holds the path to desired serial port
    SerialClient::BaudRate  m_baudrate          = SerialClient::BaudRate::BAUDRATE_INVALID;     /// Holds the baudrate
    SerialClient            m_comms;                            /// Holds the serial client

    long                    m_txCount           = 0;            /// transmit count
    long                    m_txErrorCount      = 0;            /// transmit error count
    long                    m_rxCount           = 0;            /// receive count
    long                    m_rxErrorCount      = 0;            /// receive error count

};