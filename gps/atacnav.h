#pragma once
/////////////////////////////////////////////////////////////////////////////////
// @file            atacnav.h
// @brief           A class for communicating with ATACNAV GPS modules
// @author          Chip Brommer
/////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////
//
// Includes:
//          name                            reason included
//          ------------------              ------------------------
#include <string>                           // strings
//
#include "gps_type.h"                       // base class
#include "atacnav_info.h"                   // atacnav info
//#include "../utilities/udp_client.h"        // UDP comms
// 
/////////////////////////////////////////////////////////////////////////////////

/// @brief 
struct AtacnavData
{
    unsigned long     msg5007RxCount;
    unsigned long     msg5007ChecksumErrorCount;
    unsigned long     msg5010RxCount;
    unsigned long     msg5010ChecksumErrorCount;

    Atacnav::GIG::Message5007   lastReceived5007; // whole msg for TM; most recently received (with valid checksum)
    Atacnav::GIG::Message5010   lastReceived5010; // whole msg for TM; most recently received (with valid checksum)

    double ppsTow;                 // ToV of data, GPS ToW of 1-PPS, seconds
    double lat;                    // rad
    double lon;                    // rad
    double alt;                    // m HAE
    double undulation;             // m (HAE - MSL)
    double pitch;                  // rad
    double roll;                   // rad
    double heading;                // rad
    double vel_N;                  // m/s
    double vel_E;                  // m/s
    double vel_D;                  // m/s
    double horizontalUncertainity; // m
    double verticalUncertainity;   // m
    int    gpsWeek;                // GPS Week number
    int    leapSeconds;            // leap seconds (UTC offset)
    bool   newPosDataAvailable;
    bool   msg5007DataValid;
    bool   msg5010DataValid;
    bool   dataValid;              // overall ATACNAV data is valid

};

/// @brief 
class AtacnavGps : public GpsType
{
public:

    /// @brief 
    AtacnavGps(LogClient& logger, const std::string ipAddress, const int port);

    /// @brief 
    ~AtacnavGps() {}

    /// @brief 
    int ProcessData() override;

    /// @brief 
    /// @return 
    GpsData GetCommonData() override;

protected:

private:

    /// @brief 
    void UpdateCommonData() override;

    // UDP items
    //UdpClient m_udp;

    /// Data storage
    AtacnavData m_data = {};
};