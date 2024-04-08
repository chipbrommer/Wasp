#pragma once
/////////////////////////////////////////////////////////////////////////////////
// @file            atacnav_info.h
// @brief           Definitions and structures for Atacnav modules
// @author          Chip Brommer
/////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////
//
// Includes:
//          name                            reason included
//          ------------------              ------------------------
#include <string>                           // strings
#include <cstdint>                          // std ints
#include <unordered_map>                    // map
// 
/////////////////////////////////////////////////////////////////////////////////

namespace Atacnav
{
    namespace GIG
    {
        constexpr uint8_t SYNC_1 = 0x25;
        constexpr uint8_t SYNC_2 = 0x72;
        constexpr uint8_t SYNC_3 = 0xF3;
        constexpr uint8_t SYNC_4 = 0xFA;

        enum class MSG_ID
        {
            PPS_SYNC_NAV = 5007,
            BLENDED_PPS_NAV = 5010,
        };

#pragma pack(push, 1)

        /// @brief Header for Atacnav GIG messages
        struct Header           //  offset      scale       description
        {                       //  -------     ------      ------------
            uint8_t Sync1;      //  0           n/a         = 0x25
            uint8_t Sync2;      //  1           n/a         = 0x72
            uint8_t Sync3;      //  2           n/a         = 0xF3
            uint8_t Sync4;      //  3           n/a         = 0xFA
            int16_t MessageId;  //  4-5         n/a         Message Id
            int16_t ByteCount;  //  6-7         1           Number of bytes in message including header and checksum
        };

        /// @brief Union for 32bit PPS Nav Validity flag
        union PpsNavValidity
        {
            struct
            {
                uint32_t    latLonValid         : 1;        //  bit 0, 1=valid
                uint32_t    altitudeValid       : 1;        //  bit 1, 1=valid
                uint32_t    attitudeValid       : 1;        //  bit 2, 1=valid
                uint32_t    headingValid        : 1;        //  bit 3, 1=valid
                uint32_t    velocityValid       : 1;        //  bit 4, 1=valid
                uint32_t    gpsTimeValid        : 1;        //  bit 5, 1=valid
                uint32_t    gpsHDOPValid        : 1;        //  bit 6, 1=valid
                uint32_t    gpsVDOPValid        : 1;        //  bit 7, 1=valid
                uint32_t    geoidHeightValid    : 1;        //  bit 8, 1=valid
                uint32_t    reserved            : 23;       //  bits 9-31 reserved
            }               bits;
            uint32_t        value;
        };

        /// @brief Union for 16bit Blended PPS Status information
        union BlendedPpsStatus
        {
            struct  
            {    
                uint16_t    systemMode          : 2;        // 0 = Initialization                       bits 0-1        
                                                            // 1 = Standby (Waiting for GPS)                                
                                                            // 2 = Align                                                    
                                                            // 3 = Navigation                                               
                uint16_t    solutionNavQual    : 2;         // 0=Undefined; 1=Good; 2=Marginal; 3=Poor  bits 2-3        
                uint16_t    spares1            : 4;         // = 0                                      bits 4-7        
                uint16_t    imuBitFail         : 1;         // 1 = Fail                                 bits 8          
                uint16_t    gpsBitFail         : 1;         // 1 = Fail                                 bits 9          
                uint16_t    cpuBitFail         : 1;         // 1 = Fail                                 bits 10          
                uint16_t    blendedPpsValid    : 1;         // 1 = Valid                                bits 11         
                uint16_t    spares2            : 1;         // = 0                                      bits 12         
                uint16_t    gpsPpsInvalid      : 1;         // 1 = GPS 1PPS Invalid                     bits 13         
                uint16_t    spares3            : 2;         // = 0                                      bits 14-15      
            }               bits;              
            uint16_t        value;
        };

        /// @brief Navigation Gig Message 5007 for PPS information
        struct Message5007
        {
            //  type             name               offset      units       scale       description
            //  --------------   ------------       ------      ---------   ---------   --------------------------------------------------------------
            Header              header;         //  0-7         n/a         n/a         msg ID 5007, num bytes 96
            uint32_t            MessageTOT;     //  8-11        sec         2^-14       Message Time of Transmission
            PpsNavValidity      validity;       //  12-15       n/a         n/a         validity bits
            int32_t             latitude;       //  16-19       sc          2^-31       WGS-84 Latitude at ATACNAV IMU
            int32_t             longitude;      //  20-23       sc          2^-31       WGS-84 Longitude at ATACNAV IMU
            int32_t             altitude;       //  24-27       meters      0.01        WGS-84 Altitude at ATACNAV IMU
            int32_t             pitch;          //  28-31       sc          2^-31       pitch angle
            int32_t             roll;           //  32-35       sc          2^-31       roll angle
            int32_t             heading;        //  36-39       sc          2^-31       true heading angle
            int16_t             pitchRate;      //  40-41       deg/sec     2^-6        IMU Y-Body Rate
            int16_t             rollRate;       //  42-43       deg/sec     2^-6        IMU X-Body Rate
            int16_t             headingRate;    //  44-45       deg/sec     2^-6        IMU Z-Body Rate
            uint16_t            headingError;   //  46-47       deg         2^-10       1-sigma heading error estimate
            int32_t             northVelocity;  //  48-51       m/sec       2^-18       north velocity
            int32_t             eastVelocity;   //  52-55       m/sec       2^-18       east velocity
            int32_t             downVelocity;   //  56-59       m/sec       2^-18       down velocity
            double              ppsTimeOfWeek;  //  60-67       sec         n/a         GPS Time-of-Week of 1 PPS
            uint16_t            hdop;           //  68-69       n/a         0.1         HDOP from GPS Receiver
            uint16_t            vdop;           //  70-71       n/a         0.1         VDOP from GPS Receiver
            int32_t             undulation;     //  72-75       meters      0.01        HAE - MSL
            uint8_t             spares[18];     //  76-93       n/a         n/a         spares
            uint16_t            checksum;       //  94-95       n/a         n/a         unsigned 16-bit sum of bytes 0-93
        };
    
        /// @brief Navigation Gig Message 5010 for blended information
        struct Message5010
        {
            //  type            name                offset      units       scale       description
            //  --------------  -------------       ------      ---------   ---------   --------------------------------------------------------------
            Header              header;         //  0-7         n/a         n/a         msg ID 5010, num bytes 80
            float               MessageTOV;     //  8-11        sec         1           Message Time of Transmission (UTC)
            double              gpsTowOfPps;    //  12-19       sec         1           GPS Time-of-week of last 1PPS
            double              utcTimeOfPps;   //  20-27       sec         1           UTC Time-of-day of last 1PPS
            int32_t             latitude;       //  28-31       sc          2^31        Blended Solution Latitude
            int32_t             longitude;      //  32-35       sc          2^31        Blended Solution Longitude
            int32_t             ecefPosX;       //  36-39       m           2^7         Blended Solution ECEF X
            int32_t             ecefPosY;       //  40-43       m           2^7         Blended Solution ECEF Y
            int32_t             ecefPosZ;       //  44-47       m           2^7         Blended Solution ECEF Z
            int32_t             altitudeHae;    //  48-51       m           2^7         Blended Solution Altitude in HAE
            int32_t             altitudeMsl;    //  52-55       m           2^7         Blended Solution Altitude in MSL
            int16_t             velocityNorth;  //  56-57       m/sec       2^4         Blended Solution velocity north
            int16_t             velocityEast;   //  58-59       m/sec       2^4         Blended Solution velocity east
            int16_t             velocityUp;     //  60-61       m/sec       2^4         Blended Solution velocity up
            BlendedPpsStatus    blendedStatus;  //  62-63       n/a         n/a         Bleded Solution PPS Status
            uint16_t            gpsWeek;        //  64-65       week        1           GPS Week Number
            uint32_t            utcOffset;      //  66-69       sec         1           GPS UTC Offset
            uint16_t            deltaTlsf;      //  70-71       n/a         n/a         UTC Correction Data
            uint16_t            ehe;            //  72-73       m           2^3         Blended Solution Estimated Horizontal Error
            uint16_t            eve;            //  74-75       m           2^3         Blended Solution Estimated Vertical Error
            uint16_t            epe;            //  76-77       m           2^3         Blended Solution Estimated 3D Position Error
            uint16_t            checksum;       //  78-79       n/a         n/a         unsigned 16-bit sum of bytes 0-77
        };

#pragma pack(pop)
    }
}