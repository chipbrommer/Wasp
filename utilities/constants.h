#pragma once
/////////////////////////////////////////////////////////////////////////////////
// @file            constants.h
// @brief           Various constants for ease of access in the program
// @author          Chip Brommer
/////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////
//
//  Include files:
//          name                        reason included
//          --------------------        ---------------------------------------
#include <string>                       // strings
//
#ifdef WIN32
#define NOMINMAX                        // This is needed to prevent windows from 
                                        // causing a conflict with std::min & std::max
#endif
/////////////////////////////////////////////////////////////////////////////////

constexpr double two_56 = 72057594037927936.0;      // 2^56
constexpr double two_50 =  1125899906842624.0;      // 2^50
constexpr double two_48 =   281474976710656.0;      // 2^48
constexpr double two_43 =     8796093022208.0;      // 2^43
constexpr double two_40 =     1099511627776.0;      // 2^40
constexpr double two_33 =        8589934592.0;      // 2^33
constexpr double two_32 =        4294967296.0;      // 2^32
constexpr double two_31 =        2147483648.0;      // 2^31
constexpr double two_30 =        1073741824.0;      // 2^30
constexpr double two_29 =         536870912.0;      // 2^29
constexpr double two_27 =         134217728.0;      // 2^27
constexpr double two_26 =          67108864.0;      // 2^26
constexpr double two_25 =          33554432.0;      // 2^25
constexpr double two_24 =          16777216.0;      // 2^24
constexpr double two_22 =           4194304.0;      // 2^22
constexpr double two_21 =           2097152.0;      // 2^21
constexpr double two_20 =           1048576.0;      // 2^20
constexpr double two_19 =            524288.0;      // 2^19
constexpr double two_18 =            262144.0;      // 2^18
constexpr double two_17 =            131072.0;      // 2^17
constexpr double two_16 =             65536.0;      // 2^16
constexpr double two_15 =             32768.0;      // 2^15
constexpr double two_14 =             16384.0;      // 2^14
constexpr double two_13 =              8192.0;      // 2^13
constexpr double two_12 =              4096.0;      // 2^12
constexpr double two_11 =              2048.0;      // 2^11
constexpr double two_10 =              1024.0;      // 2^10
constexpr double two_9  =               512.0;      // 2^9
constexpr double two_8  =               256.0;      // 2^8
constexpr double two_7  =               128.0;      // 2^7
constexpr double two_6  =                64.0;      // 2^6
constexpr double two_5  =                32.0;      // 2^5
constexpr double two_4  =                16.0;      // 2^4
constexpr double two_3  =                 8.0;      // 2^3
constexpr double two_2  =                 4.0;      // 2^2

constexpr int CM_TO_MM  = 10;
constexpr int MM_TO_M   = 1000;

constexpr int BUFFER_SIZE       = 1024;
constexpr int WEB_BUFFER_SIZE   = 50;
constexpr int AUTO_DISCOVERY_TIMEOUT_SECS = 10;

const std::string IP_PATTERN = "(?!127\\.0\\.0\\.1)(([1-9]|[0-9]{2}|1[0-9]{2}|2[0-4][0-9]|25[0-4])\\.)(([0-9]|[0-9]{2}|1[0-9]{2}|2[0-4][0-9]|25[0-5])\\.){2}([1-9]|[0-9]{2}|1[0-9]{2}|2[0-4][0-9]|25[0-4])";
const std::string NETMASK_PATTERN = "(255\\.){3}(0|255)|(255\\.){2}(0\\.){1}0|(255\\.){1}(0\\.){2}0|(0\\.){3}0";
const std::string GATEWAY_PATTERN = "(([0-9]|[1-9][0-9]|1[0-9][0-9]|2[0-4][0-9]|25[0-5])\\.){3}([0-9]|[1-9][0-9]|1[0-9][0-9]|2[0-4][0-9]|25[0-5])";
const std::string SAL_CODE_PATTERN("(111[1-9]|1[1-7][0-9]{2}|178[0-8])");



/////////////////////////////////////////////////////////////////////////////////
// Helper Functions 
/////////////////////////////////////////////////////////////////////////////////

static std::string ValidateSerialPort(const std::string& serialPort) 
{
    std::string result = serialPort;

#ifdef _WIN32
    // Check if the serial port string starts with "\\.\"
    if (result.substr(0, 4) != "\\\\.\\") 
    {
        // Append "\\.\"
        result = "\\\\.\\" + result;
    }
#else
    // For Linux, check if the serial port string starts with "/dev/"
    if (result.substr(0, 5) != "/dev/") 
    {
        // Append "/dev/"
        result = "/dev/" + result;
    }
#endif

    return result;
}

static void ConvertSecondsToHMS(int seconds, int& hours, int& minutes, int& secs)
{
    // Calculate hours
    hours = seconds / 3600;

    // Calculate remaining seconds after subtracting hours
    int remainingSeconds = seconds % 3600;

    // Calculate minutes
    minutes = remainingSeconds / 60;

    // Calculate remaining seconds after subtracting minutes
    secs = remainingSeconds % 60;
}