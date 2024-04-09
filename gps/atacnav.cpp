
/////////////////////////////////////////////////////////////////////////////////
// @file            atacnav.cpp
// @brief           Implementation for the atacnav GPS class
// @author          Chip Brommer
/////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////
//
// Includes:
//          name                            reason included
//          ------------------              ------------------------
#include    "atacnav.h"                     // Header
#include    <iostream>
// 
/////////////////////////////////////////////////////////////////////////////////

AtacnavGps::AtacnavGps(LogClient& logger, const std::string ipAddress, const int port) :
    GpsType("ATACNAV", logger, "", SerialClient::BaudRate::BAUDRATE_INVALID) 
{

}

int AtacnavGps::ProcessData()
{
    static unsigned int bytesInBuffer = 0;
    static unsigned char inBuffer[BUFFER_SIZE] = {};
    bool newData = false;

    // Make sure we have a good socket
    //if (!m_udp.IsGood()) return -1;

    // Attempt to read some data
    int bytesRead = 0; // m_udp.ReceiveBroadcast(inBuffer, sizeof(inBuffer));

    // If a bad read or no data, return
    if (bytesRead <= 0) return bytesRead;
   
    bytesInBuffer += bytesRead;

    // If we didnt read enough for a header, return
    if (bytesRead <= sizeof(Atacnav::GIG::Header)) return 0;

    // Attempt to process the data
    unsigned char* bufferPtr = inBuffer;
    while (bytesInBuffer >= sizeof(Atacnav::GIG::Header))
    {
        // Extract header
        Atacnav::GIG::Header* tempHeader = reinterpret_cast<Atacnav::GIG::Header*>(bufferPtr);

        // Validate sync bytes
        if ((tempHeader->Sync1 != Atacnav::GIG::SYNC_1) ||
            (tempHeader->Sync2 != Atacnav::GIG::SYNC_2) ||
            (tempHeader->Sync3 != Atacnav::GIG::SYNC_3) ||
            (tempHeader->Sync4 != Atacnav::GIG::SYNC_4))
        {
            // Incorrect sync bytes, discard the message
            bufferPtr += 1; // Move to the next byte
            bytesInBuffer -= 1;
            continue;
        }

        // Check if we have enough bytes for the whole message
        if (bytesInBuffer < tempHeader->ByteCount) 
        {
            // Insufficient data, wait for more
            break;
        }

        // Determine the message type based on MessageId
        switch (tempHeader->MessageId)
        {
        case static_cast<int>(Atacnav::GIG::MSG_ID::MSG_5007):
        {
            // Process PPS_SYNC_NAV message
            std::memcpy(&m_data.lastReceived5007, bufferPtr, sizeof(Atacnav::GIG::Message5007));
            bufferPtr += sizeof(Atacnav::GIG::Message5007);
            bytesInBuffer -= sizeof(Atacnav::GIG::Message5007);
            m_data.msg5007RxCount++;
            newData = true;
        }
        break;
        case static_cast<int>(Atacnav::GIG::MSG_ID::MSG_5010):
        {
            // Process BLENDED_PPS_NAV message
            std::memcpy(&m_data.lastReceived5010, bufferPtr, sizeof(Atacnav::GIG::Message5010));
            bufferPtr += sizeof(Atacnav::GIG::Message5010);
            bytesInBuffer -= sizeof(Atacnav::GIG::Message5010);
            m_data.msg5010RxCount++;
            newData = true;
        }
        break;
        default:
            // Unknown message type, discard the message
            bufferPtr += 1; // Move to the next byte
            bytesInBuffer -= 1;
            m_commonData.rxErrorCount++;
            continue;
        }
    }

    // Move any remaining data to the beginning of the buffer
    if (bytesInBuffer > 0 && bufferPtr != inBuffer)
        std::memmove(inBuffer, bufferPtr, bytesInBuffer);

    // Update the common data if we got new data
    if (newData)
    {
        UpdateCommonData();
        return 1;
    }

    // Default return
    return 0;
}

GpsData AtacnavGps::GetCommonData()
{
    return m_commonData;
}

void AtacnavGps::UpdateCommonData()
{
    m_commonData.latitude = m_data.lastReceived5010.latitude * two_31;
    m_commonData.longitude = m_data.lastReceived5010.longitude * two_31;
    m_commonData.altitude = m_data.lastReceived5010.altitudeHae * two_7;

    ConvertSecondsToHMS(m_data.lastReceived5010.utcTimeOfPps, m_commonData.hour, 
            m_commonData.min, m_commonData.sec);

    m_commonData.rxCount = m_data.msg5007RxCount + m_data.msg5010RxCount;
}