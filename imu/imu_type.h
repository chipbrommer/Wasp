#pragma once
/////////////////////////////////////////////////////////////////////////////////
// @file            imu_type.h
// @brief           Base class for an IMU unit for WASP 
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
#include "../utilities/log_client.h"        // logger
// 
/////////////////////////////////////////////////////////////////////////////////

/// @brief The template of common IMU data
struct ImuData
{
    double roll             = 0.0;
    double pitch            = 0.0;
    double yaw              = 0.0;
    double rollDelta        = 0.0;
    double pitchDelta       = 0.0;
    double yawDelta         = 0.0;

    bool hardwareError      = false;        // @todo fix these with some proper error types ??? 
    bool softwareError      = false;        // @todo fix these with some proper error types ??? 

};

/// @brief The base class for an IMU unit for WASP
class ImuType
{
public:

    /// @brief 
    /// @param name 
    /// @param logger 
    /// @param path 
    /// @param baudrate 
    ImuType(const std::string& name, LogClient& logger, const std::string path, const SerialClient::BaudRate baudrate) : 
        m_name(name), m_logger(logger), m_path(path), m_baudrate(baudrate)
    {
        m_comms.OpenConfigure(m_path, m_baudrate, SerialClient::ByteSize::EIGHT, SerialClient::Parity::NONE);
        m_logger.AddLog(m_name, LogClient::LogLevel::Info, "Initialized.");
    }

    /// @brief 
    ~ImuType() 
    {
        m_comms.Close();
        m_logger.AddLog(m_name, LogClient::LogLevel::Info, "Uninitialized.");
    }

    /// @brief 
    /// @return -1 on error, else number of bytes read and processed
    virtual int ProcessData() = 0;

    /// @brief 
    /// @param data - in - data to be sent
    /// @param size - in - size of data to be sent
    /// @return -1 on error, else number of bytes sent
    int Send(const std::byte* data, const size_t size)
    {
        if (!m_comms.IsOpen()) return -1;
        return m_comms.Write(data, size);
    }

    /// @brief 
    /// @return 
    ImuData GetCommonData() const { return m_commonData; }

protected:
    /// @brief 
    virtual void UpdateCommonData() = 0;

    std::string             m_name              = "";           /// name of the unit
    ImuData                 m_commonData        = {};           /// Holds common data 
    LogClient&              m_logger;
    std::string             m_path              = "";           /// Holds the path to desired serial port
    SerialClient::BaudRate  m_baudrate          = SerialClient::BaudRate::BAUDRATE_INVALID;     /// Holds the baudrate
    SerialClient            m_comms;                            /// Holds the serial client

    long                    m_txCount           = 0;            /// transmit count
    long                    m_txErrorCount      = 0;            /// transmit error count
    long                    m_rxCount           = 0;            /// receive count
    long                    m_rxErrorCount      = 0;            /// receive error count

private:

};