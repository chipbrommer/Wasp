#pragma once
/////////////////////////////////////////////////////////////////////////////////
// @file            ublox_info.h
// @brief           Definitions and structures for Ublox modules
// @author          Chip Brommer
/////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////
//
// Includes:
//          name                            reason included
//          ------------------              ------------------------
#include <string>                           // strings
#include <cstdint>                          // std ints
// 
/////////////////////////////////////////////////////////////////////////////////

namespace UbloxInfo
{

}

namespace UBX
{
    namespace Header
    {
        constexpr uint8_t SyncChar1 = 0xB5;
        constexpr uint8_t SyncChar2 = 0x62;
    }

    namespace ACK
    {
        constexpr uint8_t classId = 0x05;

        namespace ACK
        {
            constexpr uint8_t messageId = 0x01;
        }

        namespace NACK
        {
            constexpr uint8_t messageId = 0x00;
        }
    }

    namespace CFG
    {

        constexpr uint8_t classId = 0x06;

        namespace RATE 
        {
            constexpr uint8_t messageId = 0x08;

            constexpr uint8_t UTC_SOURCE = 0x00;
            constexpr uint8_t GPS_SOURCE = 0x01;
            constexpr uint8_t GLO_SOURCE = 0x02;
            constexpr uint8_t BDS_SOURCE = 0x03;
            constexpr uint8_t GAL_SOURCE = 0x04;

            constexpr uint8_t MEASURE_HZ1 = 0xE8;
            constexpr uint8_t MEASURE_HZ5 = 0xC8;
            constexpr uint8_t MEASURE_HZ10 = 0x64;

            constexpr uint8_t NAV_CYCLES1 = 0x01;
            constexpr uint8_t NAV_CYCLES2 = 0x02;
            constexpr uint8_t NAV_CYCLES3 = 0x03;
        }

        namespace VALSET
        {
            constexpr uint8_t messageId = 0x8A;
        }

        namespace MSG
        {
            constexpr uint8_t messageId = 0x01;
        }

        namespace PRT
        {
            constexpr uint8_t messageId = 0x00;
            constexpr uint8_t payloadSize = 0x14;

            namespace UART1 
            {
                constexpr uint8_t portId = 0x01;
            }
            
            namespace UART2 
            {
                constexpr uint8_t portId = 0x02;
            }
            
        }

        namespace RST
        {
            constexpr uint8_t messageId = 0x04;
        }
    }

    namespace MON
    {
        constexpr uint8_t classId = 0x0A;

        namespace VER
        {
            constexpr uint8_t messageId = 0x04;
        }
    }

    namespace NAV
    {
        constexpr uint8_t classId = 0x01;

        //! @brief holds UBX NAV COV message id, payload length, and full message length
        namespace COV
        {
            constexpr uint8_t messageId = 0x36;
            constexpr int payloadLength = 64;
            constexpr int fullMessageLength = 72;
        };

        //! @brief holds UBX NAV DOP message id, payload length, and full message length
        namespace DOP
        {
            constexpr uint8_t messageId = 0x04;
            constexpr int payloadLength = 18;
            constexpr int fullMessageLength = 26;
        };

        //! @brief holds UBX NAV SAT message id and supported message version for current firmware (1.00)
        namespace SAT
        {
            constexpr uint8_t messageId = 0x35;
            constexpr uint8_t messageVersion = 0x01;
        };

        //! @brief holds UBX NAV STATUS message id, payload length, and full message length
        namespace STATUS
        {
            constexpr uint8_t messageId = 0x03;
            constexpr int payloadLength = 16;
            constexpr int fullMessageLength = 24;
        };

        //! @brief holds UBX NAV PVT message id, payload length, and full message length
        namespace PVT
        {
            constexpr uint8_t messageId = 0x07;
            constexpr int payloadLength = 92;
            constexpr int fullMessageLength = 100;
        };

        //! @brief holds UBX NAV POSECEF message id, payload length, and full message length
        namespace POSECEF
        {
            constexpr uint8_t messageId = 0x1;
            constexpr int payloadLength = 20;
            constexpr int fullMessageLength = 28;
        };

        //! @brief holds UBX NAV POSLLH message id, payload length, and full message length
        namespace POSLLH
        {
            constexpr uint8_t messageId = 0x02;
            constexpr int payloadLength = 28;
            constexpr int fullMessageLength = 36;
        };

        //! @brief holds UBX NAV VELECEF message id, payload length, and full message length
        namespace VELECEF
        {
            constexpr uint8_t messageId = 0x11;
            constexpr int payloadLength = 20;
            constexpr int fullMessageLength = 28;
        };

        //! @brief holds UBX NAV VELNED message id, payload length, and full message length
        namespace VELNED
        {
            constexpr uint8_t messageId = 0x12;
            constexpr int payloadLength = 36;
            constexpr int fullMessageLength = 44;
        };

        //! @brief holds UBX NAV TIMEUTC message id, payload length, and full message length
        namespace TIMEUTC
        {
            constexpr uint8_t messageId = 0x21;
            constexpr int payloadLength = 20;
            constexpr int fullMessageLength = 28;
        };

        //! @brief holds UBX NAV TIMEGPS message id, payload length, and full message length
        namespace TIMEGPS
        {
            constexpr uint8_t messageId = 0x20;
            constexpr int payloadLength = 16;
            constexpr int fullMessageLength = 24;
        };
    }
}