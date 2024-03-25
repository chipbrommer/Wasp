/////////////////////////////////////////////////////////////////////////////////
// @file            imu_manager.cpp
// @brief           Implementation for the IMU manager
// @author          Chip Brommer
/////////////////////////////////////////////////////////////////////////////////
#pragma once
/////////////////////////////////////////////////////////////////////////////////
//
// Includes:
//          name                            reason included
//          ------------------              ------------------------
#include <string>                           // strings
//
#include "imu_manager.h"                    // header
// 
/////////////////////////////////////////////////////////////////////////////////


ImuManager::ImuManager(LogClient& logger, ImuOptions imu) : m_logger(logger), m_imu(imu) {}

ImuManager::~ImuManager()
{
    Stop();
}

void ImuManager::Start()
{

}

void ImuManager::Stop()
{

}