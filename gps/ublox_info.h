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
#include <unordered_map>                    // map
// 
/////////////////////////////////////////////////////////////////////////////////

namespace Ublox
{
    /// @brief constant for the size of additional version information
    constexpr int EXTENDED_VERSION_DATA_SIZE = 30;

    /// @brief enum for GNSS fix values. 
    enum class GNSS_FIX_TYPE
    {
        NO_FIX                      = 0,
        DEAD_RECK_FIX               = 1,
        TWO_D_FIX                   = 2, 
        THREE_D_FIX                 = 3,
        THREE_D_AND_DEAD_RECK_FIX   = 4,
        TIME_ONLY_FIX               = 5,
    };

    /// @brief enum class for types of reset of the ublox unit
    enum class RESET_TYPE : uint8_t
    {
        HW_RESET_IMMEDIATE              = 0x00,
        CONTROLLED_SW_RESET             = 0x01,
        CONTROLLED_SW_RESET_GNSS_ONLY   = 0x02,
        HW_RESET_AFTER_SHUTDOWN         = 0x04,
        CONTROLLED_GNSS_STOP            = 0x08,
        CONTROLLED_GNSS_START           = 0x09,
    };

    /// @brief enum class for types of start request
    enum class START_TYPE : uint16_t
    {
        HOT     = 0x0000,
        WARM    = 0x0001,
        COLD    = 0xFFFF,
    };

    /// @brief enum class for the supported Ublox SW types
    enum class SW_VERSION : int
    {
        NOT_SUPPORTED,
        ROM_SPG_5_10,
        EXT_CORE_3_01,
        EXT_CORE_4_04,
    };

    /// @brief enum for the ublox receiver to indicate the number of GPS messages to collect before sending an output message
    enum class MESSAGE_OUTPUT_RATE : uint8_t
    {
        ONE = 0x01,
        FIVE = 0x05,
        TEN = 0x0A,
    };

    /// @brief Ublox acceptable baudrates
    enum class BAUDRATE : uint32_t
    {
        RATE_9600 = 0x2580,
        RATE_38400 = 0x9600,
        RATE_115200 = 0x1C200,
    };

    /// @brief Map for string to UBLOX_SW_VERSION enum value
    static const std::unordered_map<SW_VERSION, std::string> SW_VERSION_MAP = {
        {SW_VERSION::EXT_CORE_3_01,   "EXT CORE 3.01"},
        {SW_VERSION::EXT_CORE_4_04,   "EXT CORE 4.04"},
        {SW_VERSION::ROM_SPG_5_10,    "ROM SPG 5.10"},
        {SW_VERSION::NOT_SUPPORTED,   "SUPPORT NOT GUARANTEED!"}
    };

    /// @brief enum for the dynamic model of the gnss receiver
    enum DYNAMICS : uint8_t
    {
        PORTABLE              = 0,
        STATIONARY            = 2,
        PEDESTRIAN            = 3,
        AUTOMOTIVE            = 4,
        SEA                   = 5,
        AIRBORNE_LESS_THAN_1G = 6,
        AIRBORNE_LESS_THAN_2G = 7,
        AIRBORNE_LESS_THAN_4G = 8,
        WRIST_WATCH           = 9,
        BIKE                  = 10,
    };

    /// @brief UBX related ID's
    namespace UBX
    {
        enum class GNSS_ID : uint8_t
        {
            GPS = 0,
            SBAS = 1,
            GALILEO = 2,
            BEIDOU = 3,
            IMES = 4,
            QZSS = 5,
            GLONASS = 6,
        };

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

            struct Message
            {
                uint8_t clsID;      // Class ID of the Ack or N'Ack'd message
                uint8_t msgID;      // Message ID of the Ack or N-Ack'd message
            };
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

            namespace VALDEL
            {
                constexpr uint8_t messageId = 0x8C;

                /// @brief Configuration VALDEL layers bitfield
                union Layers
                {
                    struct                              // Bit      Description
                    {                                   //------    -----------------------
                        uint8_t reserved         : 1;   // 0        reserved
                        uint8_t bbr              : 1;   // 1        1 = delete config from BBR layer
                        uint8_t flash            : 1;   // 2        1 = delete config from flash
                        uint8_t reserved2        : 5;   // 3-7      reserved
                    }       bits;
                    uint8_t  value;
                };

                /// @brief Configuration message to delete a value
                struct Message
                {
                    uint8_t version;
                    Layers layers;
                    uint8_t reserved[2];
                };
            }

            namespace VALGET
            {
                constexpr uint8_t messageId = 0x8B;

                /// @brief Configuration message to get a value
                struct Message
                {
                    uint8_t version;
                    uint8_t layer;
                    uint16_t position;
                };
            }

            namespace VALSET
            {
                constexpr uint8_t messageId = 0x8A;

                /// @brief Configuration VALSET layers bitfield
                union Layers
                {
                    struct                              // Bit      Description
                    {                                   //------    -----------------------
                        uint8_t ram              : 1;   // 0        1 = update config in the ram layer
                        uint8_t bbr              : 1;   // 1        1 = update config from BBR layer
                        uint8_t flash            : 1;   // 2        1 = update config from flash
                        uint8_t reserved         : 5;   // 3-7      reserved
                    }       bits;
                    uint8_t  value;
                };

                /// @brief Configuration message to set a value
                struct Message
                {
                    uint8_t version;
                    Layers layers;
                    uint8_t reserved[2];
                };
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

                /// @brief Configuration reset message
                struct Message
                {
                    uint16_t navBbrMask;
                    uint8_t  resetMode;
                    uint8_t  reserved;
                };
            }
        
            /// @brief Configuration configure message
            struct Message
            {
                uint32_t clearMask;
                uint32_t saveMask;
                uint32_t loadMask;
            };
        }

        namespace MON
        {
            constexpr uint8_t classId = 0x0A;

            namespace VER
            {
                constexpr uint8_t messageId = 0x04;

                /// @brief Version information message for device
                struct Message
                {
                    char swVersion[30];
                    char hwVersion[10];
                };
            }
        }

        namespace NAV
        {
            constexpr uint8_t classId = 0x01;

            //! @brief holds UBX NAV CLK information
            namespace CLK
            {
                /// @brief Navigation clock information message
                struct Message                          // Scale     Units   
                {                                       //------    --------
                    uint32_t gpsTowInMs;                //          ms
                    int32_t clockBiasInNs;              //          ns
                    int32_t clockDriftInNss;            //          ns/s
                    uint32_t timeAccuracyEstInNs;       //          ns
                    uint32_t frequencyAccuracyEstimate; //          ps/s
                };
            }

            //! @brief holds UBX NAV COV message id, payload length, and full message length
            namespace COV
            {
                constexpr uint8_t messageId = 0x36;
                constexpr int payloadLength = 64;
                constexpr int fullMessageLength = 72;

                /// @brief Navigation covariance information message
                struct Message                          // Scale     Units   
                {                                       //------    --------
                    uint32_t gpsTowInMs;                //          ms
                    uint8_t version;                    //
                    uint8_t posCovValid;                //
                    uint8_t velCovValid;                //
                    uint8_t reserved[9];                //
                    float   positionCovNN;              //          m^2
                    float   positionCovNE;              //          m^2
                    float   positionCovND;              //          m^2
                    float   positionCovEE;              //          m^2
                    float   positionCovED;              //          m^2
                    float   positionCovDD;              //          m^2
                    float   velocityCovNN;              //          m^2 / s^2
                    float   velocityCovNE;              //          m^2 / s^2
                    float   velocityCovND;              //          m^2 / s^2
                    float   velocityCovEE;              //          m^2 / s^2
                    float   velocityCovED;              //          m^2 / s^2
                    float   velocityCovDD;              //          m^2 / s^2
                };
            };

            //! @brief holds UBX NAV DOP message id, payload length, and full message length
            namespace DOP
            {
                constexpr uint8_t messageId = 0x04;
                constexpr int payloadLength = 18;
                constexpr int fullMessageLength = 26;

                /// @brief Navigation dillution of precision message
                struct Message                          // Scale     Units   
                {                                       //------    --------
                    uint32_t gpsTowInMs;                //          ms
                    uint16_t geometricDOP;              // 0.01
                    uint16_t positionDOP;               // 0.01
                    uint16_t timeDOP;                   // 0.01
                    uint16_t verticalDOP;               // 0.01
                    uint16_t horizontalDOP;             // 0.01
                    uint16_t northingDOP;               // 0.01
                    uint16_t eastingDOP;                // 0.01
                };
            };

            //! @brief holds UBX NAV ORB information
            namespace ORB
            {
                /// @brief Navigation satellite orbit information
                struct Message                          // Scale     Units   
                {                                       //------    --------
                    uint32_t gpsTowInMs;                //          ms
                    uint8_t  version;                   //
                    uint8_t  numSvDatabase;             //
                    uint8_t  reserved[2];               //
                };

                /// @brief Flags for Orbit satellite data
                union Flags
                {
                    struct                              // Bit      Description
                    {                                   //------    -----------------------
                        uint8_t health       : 2;       // 0-1      0 = unk, 1 = healthy, 2 = unhealthy
                        uint8_t visibility   : 2;       // 2-3      0 = unk, 1 = below hor, 2 = above hor, 3 = above elev mask
                        uint8_t reserved     : 4;       // 4-7      Reserved
                    }       bits;
                    uint8_t  value;
                };

                /// @brief Ephemeris data
                union Ephemeris
                {
                    struct                              // Bit      Description
                    {                                   //------    -----------------------
                        uint8_t usability    : 5;       // 0-4      31 = unk, 30 = >450mins, 29>n>1= between (n-1)*15 and n*15 minutes, 0 = cant be used
                        uint8_t source       : 3;       // 5-7      0 = not avail, 1 = gnss transmission, 2 = external aiding, 3-7 = other
                    }       bits;
                    uint8_t  value;
                };

                /// @brief Almanac data
                union Almanac
                {
                    struct                              // Bit      Description
                    {                                   //------    -----------------------
                        uint8_t usability    : 5;       // 0-4      31 = unk, 30 = >30days, 29>n>1= between (n-1) and n days, 0 = cant be used
                        uint8_t source       : 3;       // 5-7      0 = not avail, 1 = gnss transmission, 2 = external aiding, 3-7 = Other
                    }       bits;
                    uint8_t  value;
                };

                /// @brief Other otbit data available
                union Orbit
                {
                    struct                              // Bit      Description
                    {                                   //------    -----------------------
                        uint8_t usability    : 5;       // 0-4      31 = unk, 30 = >30days, 29>n>1= between (n-1) and n days, 0 = cant be used
                        uint8_t source       : 3;       // 5-7      0 = not avail, 1 = offline data, 2 = autonomous data
                    }       bits;
                    uint8_t  value;
                };

                /// @brief Satellite orbit data for a single satellite. 
                struct Satellite                        // Scale     Units   
                {                                       //------    --------
                    uint8_t gnssId;                     // 
                    uint8_t svId;                       // 
                    Flags flags;                        // 
                    Ephemeris eph;                      // 
                    Almanac alm;                        // 
                    Orbit orb;                          // 
                };
            }

            //! @brief holds UBX NAV SAT message id and supported message version for current firmware (1.00)
            namespace SAT
            {
                constexpr uint8_t messageId = 0x35;
                constexpr uint8_t messageVersion = 0x01;

                /// @brief Navigation satellite information message
                struct Message                          // Scale     Units   
                {                                       //------    --------
                    uint32_t gpsTowInMs;                //          ms
                    uint8_t version;                    //
                    uint8_t numberSvs;                  //
                    uint8_t reserved[2];                //
                };

                /// @brief Navigation satellite flags
                union Flags
                {
                    struct                              // Bit      Description
                    {                                   //------    -----------------------
                        uint32_t quality         : 3;   // 0 -2     0 = no signal
                                                        //          1 = searching 
                                                        //          2 = acquired 
                                                        //          3 = detected but unusable
                                                        //          4 = code locked and time sync'd
                                                        //          5,6,7 = code and carrier locked and time sync'd
                        uint32_t svUsed          : 1;   // 3        1 = used for navigation
                        uint32_t health          : 2;   // 4-5      0 = unk, 1 = healthy, 2 = unhealthy
                        uint32_t diffCorr        : 1;   // 6        1 = differnetial correct data available
                        uint32_t smoothed        : 1;   // 7        1 = carrier smoothed pseudo range used
                        uint32_t orbitSource     : 3;   // 8-10     0 = no orbit information is available for this SV
                                                        //          1 = ephemeris is used
                                                        //          2 = almanac is used
                                                        //          3 = AssistNow Offline orbit is used
                                                        //          4 = AssistNow Autonomous orbit is used
                                                        //          5,6,7 = other orbit information is used
                        uint32_t ephAvail        : 1;   // 11       1 = ephemeris availabke
                        uint32_t almAvail        : 1;   // 12       1 = almanac available
                        uint32_t anoAvail        : 1;   // 13       1 = AssistNow Offline available
                        uint32_t aopAvail        : 1;   // 14       1 = AssistNow Autonomous available
                        uint32_t reserved        : 1;   // 15
                        uint32_t sbasCorrUsed    : 1;   // 16       1 = SBAS correction used
                        uint32_t rtcmCorrUsed    : 1;   // 17       1 = RTCM correction used
                        uint32_t slasCorrUsed    : 1;   // 18       1 = QZSS SLAS correction used
                        uint32_t spartnCorrUsed  : 1;   // 19       1 = SPARTN correction used
                        uint32_t prCorrUsed      : 1;   // 20       1 = Pseudorange correction used
                        uint32_t crCorrUsed      : 1;   // 21       1 = carrier correction used
                        uint32_t doCorrUsed      : 1;   // 22       1 = coppler correction used
                        uint32_t clasCorrUsed    : 1;   // 23       1 = CLAS correction used
                    }       bits;
                    uint32_t value;
                };

                /// @brief Navigation satellite data for a single satellite. 
                struct Satellite                        // Scale     Units  
                {                                       //------    --------
                    uint8_t gnssId;                     //          
                    uint8_t satelliteId;                //
                    uint8_t carrierToNoiseRatioInDbhz;  //          dbhz
                    int8_t  elevationInDeg;             //          deg
                    int16_t azimuthInDeg;               //          deg
                    int16_t prResidual;                 // 0.1      m
                    Flags flags;                        //

                    Satellite& operator = (const Satellite& other)
                    {
                        if (this != &other) 
                        {
                            gnssId                      = other.gnssId;
                            satelliteId                 = other.satelliteId;
                            carrierToNoiseRatioInDbhz   = other.carrierToNoiseRatioInDbhz;
                            elevationInDeg              = other.elevationInDeg;
                            azimuthInDeg                = other.azimuthInDeg;
                            prResidual                  = other.prResidual;
                            flags                       = other.flags;
                        }
                        return *this;
                    }

                    bool operator == (const Satellite& rhs) const
                    {
                        return (
                            this->gnssId        == rhs.gnssId &&
                            this->satelliteId   == rhs.satelliteId
                            );
                    }

                    bool operator != (const Satellite& rhs) const
                    {
                        return !(*this == rhs);
                    }
                };
            };

            //! @brief holds UBX NAV STATUS message id, payload length, and full message length
            namespace STATUS
            {
                constexpr uint8_t messageId = 0x03;
                constexpr int payloadLength = 16;
                constexpr int fullMessageLength = 24;

                /// @brief Navigation Status flag
                union StatusFlag
                {
                    struct                              // Bit      Description
                    {                                   //------    -----------------------
                        uint8_t gpsFixOk         : 1;   // 0        1 = position and velocity valid
                        uint8_t diffSolution     : 1;   // 1        1 = differential corrections applied
                        uint8_t wknSet           : 1;   // 2        1 = week number valid
                        uint8_t towSet           : 1;   // 3        1 = time of week valid
                        uint8_t reserved         : 4;   // 4-7      reserved
                    }       bits;
                    uint8_t  value;
                };

                /// @brief Navigation status fix flag
                union StatusFix
                {
                    struct                              // Bit      Description
                    {                                   //------    -----------------------
                        uint8_t diffCorr         : 1;   // 0        1 = differential corrections available
                        uint8_t weekValid        : 1;   // 1        1 = valid carrier solution
                        uint8_t reserved         : 5;   // 2-5      reserved
                        uint8_t mapMatching      : 2;   // 6-7      0 = none, 1 = valid but not used, 10 = valid and used, 11 = valid and used w/ dead reck. 
                    }       bits;
                    uint8_t  value;
                };

                /// @brief Navigation Status flag 2
                union StatusFlag2
                {
                    struct                              // Bit      Description
                    {                                   //------    -----------------------
                        uint8_t psmState         : 2;   // 0-1      1 = valid time of week
                        uint8_t reserved         : 1;   // 2        reserved
                        uint8_t spoofDetState    : 1;   // 3-4      1 = valid week number
                        uint8_t reserved2        : 1;   // 5        reserved
                        uint8_t carrierSolution  : 5;   // 6-7      0 = no carrier solution, 1 = solution with floating ambiguities, 2 = solution with fixed ambiguities
                    }       bits;
                    uint8_t  value;
                };

                /// @brief Navigation status message
                struct Message                          // Scale     Units  
                {                                       //------    --------
                    uint32_t gpsTowInMs;                //          ms
                    uint8_t gpsFix;                     //      
                    StatusFlag flags;                   //
                    StatusFix fixStatus;                //
                    StatusFlag2 flags2;                 //
                    uint32_t timeToFirstFixInMs;        //          ms
                    uint32_t mSecsSinceStartup;         //          ms
                };
            };

            //! @brief holds UBX NAV PVT message id, payload length, and full message length
            namespace PVT
            {
                constexpr uint8_t messageId = 0x07;
                constexpr int payloadLength = 92;
                constexpr int fullMessageLength = 100;

                /// @brief PVT validity flags
                union ValidityFlags
                {
                    struct                              // Bit      Description
                    {                                   //------    -----------------------
                        uint8_t validDate        : 1;   // 0        1 = valid
                        uint8_t validTime        : 1;   // 1        1 = valid
                        uint8_t fullyResolved    : 1;   // 2        1 = fully resolved
                        uint8_t validMagDec      : 1;   // 3        1 = valid
                        uint8_t reserved         : 4;   // 4-7      reserved
                    }       bits;
                    uint8_t  value;
                };

                /// @brief PVT status flags
                union StatusFlags
                {
                    struct                              // Bit      Description
                    {                                   //------    -----------------------
                        uint8_t gnssFixOk        : 1;   // 0        1 = valid fix
                        uint8_t diffSoln         : 1;   // 1        1 = corrections applied
                        uint8_t psmState         : 3;   // 2-4      0 = off, 1 = on, 2 = acquisition, 3 = Tracking, 4 = optimized tracking, 5 = inactive
                        uint8_t validHeading     : 1;   // 5        1 = valid
                        uint8_t carrierSoln      : 2;   // 6-7      0 = no range solution, 1 = floating ambiguities, 2 = fixed ambiguities
                    }       bits;
                    uint8_t  value;
                };

                /// @brief PVT confirmation flags
                union ConfirmationFlags
                {
                    struct                              // Bit      Description
                    {                                   //------    -----------------------
                        uint8_t reserved         : 5;   // 0-4      reserved
                        uint8_t available        : 1;   // 5        1 = utc date and time confirmed available
                        uint8_t date             : 1;   // 6        1 = utc date validity could be confirmed
                        uint8_t time             : 1;   // 7        1 = utc time validity could be confirmed
                    }       bits;
                    uint8_t  value;
                };

                /// @brief PVT additional flags
                union AdditionalFlags
                {
                    struct                              // Bit      Description
                    {                                   //------    -----------------------
                        uint16_t invalidLLH      : 1;   // 0        1 = invalid lon, lat, hae and msl
                        uint16_t available       : 4;   // 1-4      0 = not available
                                                        //          1 = utc date and time confirmed available
                                                        //          2 = Age between 1 (inclusive) and 2 seconds
                                                        //          3 = Age between 2 (inclusive) and 5 seconds
                                                        //          4 = Age between 5 (inclusive) and 10 seconds
                                                        //          5 = Age between 10 (inclusive) and 15 seconds
                                                        //          6 = Age between 15 (inclusive) and 20 seconds
                                                        //          7 = Age between 20 (inclusive) and 30 seconds
                                                        //          8 = Age between 30 (inclusive) and 45 seconds
                                                        //          9 = Age between 45 (inclusive) and 60 seconds
                                                        //          10 = Age between 60 (inclusive) and 90 seconds
                                                        //          11 = Age between 90 (inclusive) and 120 seconds
                                                        //          >=12 = Age greater or equal than 120 seconds
                        uint16_t reserved        : 8;   // 5-12     reserved
                        uint16_t authTime        : 1;   // 13       1 = time authenticated
                        uint16_t reserved2       : 2;   // 14-15    reserved
                    }       bits;
                    int16_t value;
                };

                /// @brief Navigation Position Velocity and Time message
                struct Message                          // Scale     Units              
                {                                       //------    --------
                    uint32_t gpsTowInMs;                //          ms
                    uint16_t year;                      //          
                    uint8_t month;                      //          
                    uint8_t day;                        //          
                    uint8_t hour;                       //          
                    uint8_t min;                        //          
                    uint8_t sec;                        //
                    ValidityFlags validFlags;           //
                    uint32_t timeAccuracyEstimate;      //          ns
                    int32_t nanoSeconds;                //          ns
                    uint8_t fixType;                    //
                    StatusFlags flags;                  //
                    ConfirmationFlags flags2;           //
                    uint8_t numSv;                      //
                    int32_t longitudeInDeg;             // 1e-7     deg
                    int32_t latitudeInDeg;              // 1e-7     deg
                    int32_t haeInMm;                    //          mm
                    int32_t heightMslInMm;              //          mm
                    uint32_t horizontalAccuracyEstInMm; //          mm
                    uint32_t verticalAccuracyEstInMm;   //          mm
                    int32_t velocityNorthInMms;         //          mm/s
                    int32_t velocityEastInMms;          //          mm/s
                    int32_t velocityDownInMms;          //          mm/s
                    int32_t groundSpeedInMms;           //          mm/s
                    int32_t headingMotionInDeg;         // 1e-5     deg
                    uint32_t speedAccuracyEstInMms;     //          mm/s
                    uint32_t headingAccuracyEstInDeg;   // 1e-5     deg
                    uint16_t positionDOP;               // 0.01
                    AdditionalFlags flags3;             //
                    uint8_t reserved[4];                //
                    int32_t headingVehicle;             // 1e-5     deg
                    int16_t magDeclination;             // 1e-2     deg
                    uint16_t magneticAccuracyEstimate;  // 1e-2     deg
                };
            };

            //! @brief holds UBX NAV POSECEF message id, payload length, and full message length
            namespace POSECEF
            {
                constexpr uint8_t messageId = 0x1;
                constexpr int payloadLength = 20;
                constexpr int fullMessageLength = 28;

                /// @brief Position ECEF message
                struct Message                          // Scale     Units   
                {                                       //------    --------
                    uint32_t gpsTowInMs;                //          ms
                    int32_t ecefX;                      //          cm
                    int32_t ecefY;                      //          cm
                    int32_t ecefZ;                      //          cm
                    uint32_t posAccuracyEstimate;       //          cm
                };
            };

            //! @brief holds UBX NAV POSLLH message id, payload length, and full message length
            namespace POSLLH
            {
                constexpr uint8_t messageId = 0x02;
                constexpr int payloadLength = 28;
                constexpr int fullMessageLength = 36;

                /// @brief Position LLH message
                struct Message                          // Scale     Units
                {                                       //------    --------
                    uint32_t gpsTowInMs;                //          ms
                    int32_t longitude;                  // 1e-7     deg
                    int32_t latitude;                   // 1e-7     deg
                    int32_t hae;                        //          mm
                    int32_t heightMSL;                  //          mm
                    uint32_t horizontalAccuracyEst;     //          mm
                    uint32_t verticalAccuracyEst;       //          mm
                };
            };

            //! @brief holds UBX NAV VELECEF message id, payload length, and full message length
            namespace VELECEF
            {
                constexpr uint8_t messageId = 0x11;
                constexpr int payloadLength = 20;
                constexpr int fullMessageLength = 28;

                /// @brief Navigation velocity ECEF data
                struct Message                          // Scale     Units   
                {                                       //------    --------
                    uint32_t gpsTowInMs;                //          ms
                    int32_t ecefXVelocityInCms;         //          cm/s
                    int32_t ecefYVelocityInCms;         //          cm/s
                    int32_t ecefZVelocityInCms;         //          cm/s
                    uint32_t speedAccuraryEstimateInCms;//          cm/s
                };
            };

            //! @brief holds UBX NAV VELNED message id, payload length, and full message length
            namespace VELNED
            {
                constexpr uint8_t messageId = 0x12;
                constexpr int payloadLength = 36;
                constexpr int fullMessageLength = 44;

                //! @brief Navigation velocity NED data
                struct Message                          // Scale     Units   
                {                                       //------    --------
                    uint32_t gpsTowInMs;                //          ms
                    int32_t velocityNorthInCms;         //          cm/s
                    int32_t velocityEastInCms;          //          cm/s
                    int32_t velocityDownInCms;          //          cm/s
                    uint32_t speedInCms;                //          cm/s
                    uint32_t groundSpeedInCms;          //          cm/s
                    int32_t headingOfMotionInDeg;       // 1e-5     deg
                    uint32_t speedAccuracyEstInCms;     //          cm/s
                    uint32_t headingAccuracyEstInDeg;   // 1e-5     deg
                };
            };

            //! @brief holds UBX NAV TIMEUTC message id, payload length, and full message length
            namespace TIMEUTC
            {
                constexpr uint8_t messageId = 0x21;
                constexpr int payloadLength = 20;
                constexpr int fullMessageLength = 28;

                /// @brief Navigation time UTC validity flag
                union ValidityFlag
                {
                    struct                              // Bit      Description
                    {                                   //------    -----------------------
                        uint8_t validTOW : 1;           // 0        1 = valid time of week
                        uint8_t validWKN : 1;           // 1        1 = valid week number
                        uint8_t validUTC : 1;           // 2        1 = valid utc time
                        uint8_t authStatus : 1;         // 3        1 = authenticated
                        uint8_t utcStandard : 4;        // 4-7      0 = information not available
                                                        //          1 = CRL - Tokyo, japan
                                                        //          2 = NIST
                                                        //          3 = USNO
                                                        //          4 = BIPM
                                                        //          5 = European Labs
                                                        //          6 = Former Sov. Union
                                                        //          7 = NTSC - China
                                                        //          8 = NPLI
                                                        //          15 = Unknown
                    }       bits;
                    int8_t  value;
                };

                /// @brief Navigation time UTC data
                struct Message                          // Scale     Units   
                {                                       //------    --------
                    uint32_t gpsTowInMs;                //          ms
                    uint32_t timeAccuraryEstimateInNs;  //          ns
                    int32_t nanoSeconds;                //          ns
                    uint16_t year;                      //          
                    uint8_t month;                      //          
                    uint8_t day;                        //          
                    uint8_t hour;                       //          
                    uint8_t minute;                     //          
                    uint8_t seconds;                    //          
                    ValidityFlag validityFlag;          //         
                };
            };

            //! @brief holds UBX NAV TIMEGPS message id, payload length, and full message length
            namespace TIMEGPS
            {
                constexpr uint8_t messageId = 0x20;
                constexpr int payloadLength = 16;
                constexpr int fullMessageLength = 24;

                /// @brief Navigation time GPS validity flag
                union ValidityFlag
                {
                    struct                              // Bit      Description
                    {                                   //------    -----------------------
                        uint8_t towValid         : 1;   // 0        1 = valid time of week
                        uint8_t weekValid        : 1;   // 1        1 = valid week number
                        uint8_t leapSValid       : 1;   // 2        1 = valid leap seconds
                        uint8_t reserved         : 5;   // 3-7      reserved
                    }       bits;
                    uint8_t  value;
                };

                /// @brief Navigation time GPS data     
                struct Message                          // Scale     Units  
                {                                       //------    --------
                    uint32_t	gpsTowInMs;             //          ms
                    int32_t		gpsTowFractionInNs;     //          ns
                    int16_t		weekNumber;             //
                    int8_t		leapSeconds;            //          s
                    ValidityFlag validityFlag;          //
                    uint32_t	timeAccuracyInNs;       //          ns
                };

            };
        }
    }

    /// @brief NMEA related ID's
    namespace NMEA
    {
        constexpr uint8_t syncChar = 0x24;
        constexpr uint8_t classId = 0xF0;
        constexpr uint8_t endCheck1 = 0x0D;
        constexpr uint8_t endCheck2 = 0x0A;

        namespace GxGGA
        {
            constexpr uint8_t messageId = 0x00;
        }

        namespace GxGLL
        {
            constexpr uint8_t messageId = 0x01;
        }

        namespace GxGSA
        {
            constexpr uint8_t messageId = 0x02;
        }

        namespace GxGSV
        {
            constexpr uint8_t messageId = 0x03;
        }

        namespace GxRMC
        {
            constexpr uint8_t messageId = 0x04;
        }

        namespace GxVTG
        {
            constexpr uint8_t messageId = 0x05;
        }

        namespace GxGST
        {
            constexpr uint8_t messageId = 0x07;
        }

        /// @brief Holds Satellite data from UBlox NMEA messages
        struct Satellite
        {
            // variables
            int numberID = 0;
            std::string signalID;
            std::string type;
            double range = 0;
            double latitudeInDegrees = 0;
            char latitudeDirection = 'X';
            double longitudeInDegrees = 0;
            char longitudeDirection = 'X';
            int positionFixQuality = 0;
            double elevationInDegrees = 0;
            double azimuthInDegrees = 0;
            double signalStrength = 0;
            bool tracking = false;
            double utcTimeOfLastData = 0;

            // members
            bool operator == (const Satellite& s) const 
            {
                return numberID == s.numberID;
            }
        };

        /// @brief Holds fix data for NMEA satellite
        struct FixData
        {
            double utcTime = 0;												// utc time
            double latitudeInDegrees = 0;									// latitude in degreees
            char latitudeDirection = 'X';									// north or south
            double longitudeInDegrees = 0;									// longitude in degrees
            char longitudeDirection = 'X';									// east or west
            int positionFixQuality = 0;										// position fix quality
            int numberSatellitesUsed = 0;									// num of sats used 0-12
            double horizontalDOP = 0;										// HDOP
            double altitudeASLinMeters = 0;									// alt above sea level
            double geoidSeperationInMeters = 0;								// difference between ellipsoid and mean sea level
            double differentialAge = 0;										// age of the correction
            double differentialStationId = 0;								// station id providing correction
        };
    }

}
