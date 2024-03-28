#pragma once
/////////////////////////////////////////////////////////////////////////////////
// @file            cot_utility.h
// @brief           A class for generating and parsing CoT messages
// @author          Chip Brommer
/////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////
//
//  Include files:
//          name                            reason included
//          --------------------            ------------------------------------
#include <iostream>                         // ostream
#include <iomanip>                          // setw
#include <unordered_map>                    // maps
//
#include "cot_info.h"                       // schemas
#include "../external/pugixml/pugixml.hpp"  // XML
// 
/////////////////////////////////////////////////////////////////////////////////


class COT_Utility
{
public:

    /// @brief Default Construtor
    COT_Utility();

    /// @brief Default Deconstructor
    ~COT_Utility();

    /// @brief Verify a string buffer is XML
    /// @param buffer - [in] - string buffer to be verified
    /// @return - true if good XML, false if not.
    bool VerifyXML(std::string& buffer);

    /// @brief Create an XML COT message from a schema
    /// @param cot - [in] - COT Schema to be created into a message
    /// @param callsign - [in/opt] - callsign if desired. 
    /// @return std::string containing the xml message. 
    std::string GenerateXMLCOTMessage(COTSchema& cot, std::string callsign = "");

    /// @brief Parse a COT Message from std::string
    /// @param buffer  - [in]  - String buffer containing the XML data to be parsed.
    /// @param cot     - [out] - Vector of COT Structures to store the parsed data into. 
    /// @return -1 on error, 1 on good parse.
    int ParseCOT(std::string& buffer, COTSchema& cot);

    /// @brief Overloaded - Parse a COT Message from uint8_t buffer
    /// @param buffer  - [in]  - uint8_t buffer containing the XML data to be parsed.
    /// @param Targets - [out] - Vector of COT Structures to store the parsed data into. 
    /// @return -1 on error, 1 on good parse.
    int ParseCOT(const uint8_t* buffer, COTSchema& cot);

    /// @brief Parse a COT Message
    /// @param Buffer  - [in]  - uint8_t buffer containing the XML data to be parsed.
    /// @return A COT Structures containing the parsed data, use "COTSchema.Valid()" function for verify validity. 
    COTSchema ParseBufferToCOT(const uint8_t* buffer);

protected:
private:

    /// @brief Parse a string "type" attriubute
    /// @param type - [in]  - Type string to be parsed
    /// @param ind  - [out] - enumeration value for the PointType parsed from string.
    /// @param loc  - [out] - enumeration value for the LocationType parsed from string.
    /// @return true if parsed, false if not
    bool ParseTypeAttribute(std::string& type, Point::Type& ind, Location::Type& loc);

    /// @brief Parse a string "how" attriubute
    /// @param type - [in]  - "How" string to be parsed
    /// @param how  - [out] - enumeration value for the HowEntryType parsed from string.
    /// @param data - [out] - enumeration value for the HowDataType parsed from string.
    /// @return true if parsed, false if not
    bool ParseHowAttribute(std::string& type, HowEntry::Type& how, HowData::Type& data);

    /// @brief Parse a string "time" attriubute
    /// @param type - [in]  - Time string to be parsed
    /// @param dt   - [out] - DateTime struct to store the parsed data into.
    /// @return true if parsed, false if not
    bool ParseTimeAttribute(std::string& type, DateTime& dt);

    /// @brief Parse a string date stamp
    /// @param type - [in]  - Date string to be parsed
    /// @param dt   - [out] - DateTime struct to store the parsed data into.
    /// @return true if parsed, false if not
    bool ParseDateStamp(std::string& type, DateTime& dt);

    /// @brief Parse a string time stamp
    /// @param type - [in]  - Time string to be parsed
    /// @param dt   - [out] - DateTime struct to store the parsed data into.
    /// @return true if parsed, false if not
    bool ParseTimeStamp(std::string& type, DateTime& dt);

    /// @brief Converts a string into a RootType enumeration value
    /// @param root - [in] - string to be converted.
    /// @return RootType enum conversion
    Root::Type RootTypeCharToEnum(std::string& root);

    /// @brief Converts a string into a RootType enumeration value
    /// @param root - [in] - string to be converted.
    /// @return RootType enum conversion
    Point::Type PointTypeCharToEnum(std::string& type);

    /// @brief Converts a string into a RootType enumeration value
    /// @param root - [in] - string to be converted.
    /// @return RootType enum conversion
    Location::Type LocationTypeCharToEnum(std::string& loc);

    /// @brief Converts a string into a RootType enumeration value
    /// @param root - [in] - string to be converted.
    /// @return RootType enum conversion
    HowEntry::Type HowEntryTypeCharToEnum(std::string& entry);

    /// @brief Converts a string into a RootType enumeration value
    /// @param root  - [in] - string to be converted.
    /// @param entry - [in] - Entry Type to correspond to proper data type. 
    /// @return RootType enum conversion
    HowData::Type HowDataTypeCharToEnum(std::string& data, HowEntry::Type entry);
};
