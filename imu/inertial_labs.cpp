#include "inertial_labs.h"

InertialLabs::InertialLabs() : ImuType("") {}

InertialLabs::~InertialLabs() {}

int InertialLabs::ReadData()
{
    return 0;
}

void InertialLabs::ProcessData()
{

}

int InertialLabs::SendData()
{
    return 0;
}

void InertialLabs::UpdateCommonData()
{
    m_commonData.pitch = m_data.pitch;
    m_commonData.yaw = m_data.yaw;
    m_commonData.hardwareError = m_data.error;
}