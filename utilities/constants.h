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

constexpr int CM_TO_MM = 10;
constexpr int MM_TO_M = 1000;

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