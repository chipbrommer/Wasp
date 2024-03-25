#include "imu_type.h"

ImuType::ImuType() : m_port(""), m_baudrate(0.0) {}

ImuType::ImuType(std::string port, double baudrate) : m_port(port), m_baudrate(baudrate) {}

ImuType::~ImuType()
{
    Disconnect();
}

bool ImuType::Configure(std::string port, double baudrate)
{
    m_port = port;
    m_baudrate = baudrate;

    return false;
}

bool ImuType::ReconfigureReconnect(std::string port, double baudrate)
{
    Disconnect();
    Configure(port, baudrate);
    return Connect();
}

bool ImuType::Connect()
{
    return false;
}

void ImuType::Disconnect()
{

}

ImuData ImuType::GetCommonData()
{
    return m_commonData;
}

void ImuType::UpdateCommonData()
{

}