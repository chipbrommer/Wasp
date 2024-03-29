
/////////////////////////////////////////////////////////////////////////////////
// @file            ublox.cpp
// @brief           Implementation for the ublox GPS class
// @author          Chip Brommer
/////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////
//
// Includes:
//          name                            reason included
//          ------------------              ------------------------
#include    "ublox.h"                       // Header
// 
/////////////////////////////////////////////////////////////////////////////////

Ublox::Ublox(LogClient & logger, const std::string path, const SerialClient::BaudRate baudrate) :
    GpsType("UBLOX", logger, path, baudrate) 
{

}

int Ublox::ProcessData()
{
    return -1;
}

void Ublox::UpdateCommonData()
{

}