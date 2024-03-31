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
/////////////////////////////////////////////////////////////////////////////////

constexpr int cm_to_mm = 10;

constexpr int BUFFER_SIZE = 1024;

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