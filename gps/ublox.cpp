
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
#include <array>							// array
//
#include "ublox.h"							// Header
// 
/////////////////////////////////////////////////////////////////////////////////

UbloxGps::UbloxGps(LogClient & logger, const std::string path, const SerialClient::BaudRate baudrate) :
    GpsType("UBLOX", logger, path, baudrate) 
{
	m_logger.AddLog(m_name, LogClient::LogLevel::Info, "Initializing.");

	bool success = false;

	// Attempt to auto discover if we received auto
	if (m_baudrate != SerialClient::BaudRate::BAUDRATE_AUTO && m_baudrate != SerialClient::BaudRate::BAUDRATE_INVALID)
	{
		success = m_comms.OpenConfigure(m_path, m_baudrate, SerialClient::ByteSize::EIGHT, SerialClient::Parity::NONE, SerialClient::StopBits::ONE);
	}
	else if (m_baudrate == SerialClient::BaudRate::BAUDRATE_AUTO)
	{
		success = AutoDiscoverBaudRate();
	}

	if (!success)
	{
		m_logger.AddLog(m_name, LogClient::LogLevel::Error, "Initialized Failed");
		m_initialized = false;
		return;
	}

	// Initialzie the Ublox receiver
	Initialize();
}

int UbloxGps::ProcessData()
{
	static unsigned int bytesInBuffer = 0;
	static bool first = true;
	int bytesRead = 0;
	int bytesAvail = 0;
	bool newData = false;

	// Buffers 
	static uint8_t inBuffer[BUFFER_SIZE] = {};
	static uint8_t msgBuffer[BUFFER_SIZE] = {};

	// dump existing data
	if (first)
	{
		first = false;
		m_comms.Flush();
		bytesInBuffer = 0;
	}

	// if here and buffer is full for some odd reason, as a precaution let's clear the buffer and stream before proceeding
	if (bytesInBuffer >= BUFFER_SIZE)
	{
		std::fill(std::begin(inBuffer), std::end(inBuffer), 0);
		m_comms.Flush();
		bytesInBuffer = 0;
	}

	// check how many bytes are available - if none, or if no data available in buffer 
	// then continue to the next iteration of the loop
	bytesAvail = (BUFFER_SIZE - bytesInBuffer);

	// read bytes from port into buffer - only read in the max amount
	bytesRead = m_comms.Read(reinterpret_cast<std::byte*>(&inBuffer[bytesInBuffer]), bytesAvail);

	// update bytes in buffer
	if (bytesRead > 0)
	{
		bytesInBuffer += bytesRead;
	}
	else if (bytesRead < 0) return -1;
	else return 0;

	// do we have at least 2 bytes in buffer (size of sync bytes)
	while (bytesInBuffer >= Ublox::NUM_SYNC_BYTES)
	{
		bool ubxFound = false;
		bool nmeaFound = false;
		unsigned int nmeaMsgLength = 0;
		unsigned int fullMsgLength = 0;
		size_t index = 0;
		size_t index2 = 0;

		// find the start of the message
		for (index = 0; index <= (bytesInBuffer - Ublox::NUM_SYNC_BYTES); index++)
		{
			// look for the ubx sync characters
			if ((inBuffer[index + 0] == static_cast<uint8_t>(Ublox::UBX::Header::SyncChar1)) &&
				(inBuffer[index + 1] == static_cast<uint8_t>(Ublox::UBX::Header::SyncChar2)))
			{
				ubxFound = true;
				break;
			}

			// look for the nmea sync character
			else if (inBuffer[index + 0] == Ublox::NMEA::syncChar)
			{
				// if it's an nmea message, we have to find the tail manually. These are variable length.
				for (index2 = index; index2 < (bytesInBuffer - 2); index2++)
				{
					if (inBuffer[index2 + 0] == Ublox::NMEA::endCheck1 &&
						inBuffer[index2 + 1] == Ublox::NMEA::endCheck2)
					{
						nmeaMsgLength = static_cast<unsigned int>(index2 - index);
						nmeaFound = true;
						m_data.NmeaRxCount++;
						break;
					}
				}

				// leave the for loop if it's found. 
				if (nmeaFound) break;
			}
		}

		// move start of message to the front of the buffer
		if (index > 0)
		{
			std::memmove(&inBuffer[0], &inBuffer[index], (bytesInBuffer - index));
			bytesInBuffer -= static_cast<unsigned int>(index);
		}

		// if we didn't find a start of message, try again next time
		if (!ubxFound && !nmeaFound)
		{
			return 0;
		}

		// did we find a UBX message ? 
		if (ubxFound)
		{
			// 8 bytes = (sync1, sync2, class id, msg id, length(2 bytes), checksum A, checksum B)
			fullMsgLength = CalculatePayloadLength(inBuffer[4], inBuffer[5]) + 8;

			// do we have enough bytes for the message?
			// if not, continue to the top of the loop and collect more data
			if (bytesInBuffer < fullMsgLength)
			{
				return 0;
			}

			// attempt to validate the UBX checksum
			if (ValidateUbxChecksum(inBuffer, fullMsgLength) == 0)
			{
				// checksum fail, dump the sync bytes and continue searching
				m_data.ChecksumFailCount++;
				std::memmove(&inBuffer[0], &inBuffer[Ublox::NUM_SYNC_BYTES], (bytesInBuffer - Ublox::NUM_SYNC_BYTES));
				bytesInBuffer -= Ublox::NUM_SYNC_BYTES;
				continue;
			}

			// consume the UBX message
			if (bytesInBuffer >= fullMsgLength)
			{
				// clear the msg buffer 
				std::fill(std::begin(msgBuffer), std::end(msgBuffer), 0);
				// copy the memory to msg buffer
				std::copy(&inBuffer[0], &inBuffer[fullMsgLength], &msgBuffer[0]);
				// move the data in the buffer up.
				std::memmove(&inBuffer[0], &inBuffer[fullMsgLength], bytesInBuffer - fullMsgLength);
				// update Bytes in buffer
				bytesInBuffer -= fullMsgLength;
			}

			// handle the UBX message 
			HandleUbxMessage(msgBuffer);
			m_data.UbxRxCount++;
			newData = true;
		}
		// did we find an NMEA message ? 
		else if (nmeaFound)
		{
			if (nmeaMsgLength > sizeof(msgBuffer))
			{
				// Message is too large for msgBuffer; dump the sync byte and continue searching
				m_data.ChecksumFailCount++;
				std::memmove(&inBuffer[0], &inBuffer[1], (bytesInBuffer - 1));
				bytesInBuffer -= 1;
				continue;
			}

			// clear the msg buffer 
			std::fill(std::begin(msgBuffer), std::end(msgBuffer), 0);
			// copy the memory to msg buffer
			std::copy(&inBuffer[0], &inBuffer[nmeaMsgLength], &msgBuffer[0]);
			// move the data in the buffer up.
			// nmeaMsgLength does not include the ending "\n\r", so +2 for those characters that follow an NMEA message
			std::memmove(&inBuffer[0], &inBuffer[nmeaMsgLength + 2], bytesInBuffer - (nmeaMsgLength + 2));
			// update Bytes in buffer
			bytesInBuffer -= (nmeaMsgLength + 2);

			// For NMEA we are only incrementing the counts for now. 
			// @note - if desire to handle NMEA in future - use this "HandleNmeaMessage(msgBuffer);"
			// after count incrementation. 
			m_data.NmeaRxCount++;
			newData = true;
		}
	}

	if (newData) return 1;
	else return 0;
}

int	UbloxGps::RestartDevice(Ublox::START_TYPE start, Ublox::RESET_TYPE reset)
{
#pragma pack(push, 1)
	struct
	{
		uint8_t		sync1;
		uint8_t		sync2;
		uint8_t		classID;
		uint8_t		messageID;
		uint16_t	length;
		Ublox::UBX::CFG::RST::Message data = { 0 };
		uint16_t	checksum;
	} msg{};
#pragma pack(pop)

	msg.sync1				= Ublox::UBX::Header::SyncChar1;		// set sync bytes
	msg.sync2				= Ublox::UBX::Header::SyncChar2;
	msg.classID				= Ublox::UBX::CFG::classId;				// set class id byte
	msg.messageID			= Ublox::UBX::CFG::RST::messageId;		// set message id byte			
	msg.length				= 0x04;									// set message payload length
	msg.data.navBbrMask		= static_cast<uint16_t>(start);			// desired start type
	msg.data.resetMode		= static_cast<uint8_t>(reset);			// desired reset type
	msg.data.reserved		= 0x00;									// reserved
	
	// Cast each byte of 'msg' to std::byte and construct a pointer to it
	uint8_t* bufferPointer = reinterpret_cast<uint8_t*>(&msg);
	SetUbxChecksum(bufferPointer);

	int rtn = m_comms.Write(reinterpret_cast<std::byte*>(&msg), sizeof(msg));

	// write buffer to the ublox device
	if (rtn < 0)
	{
		// if error, notify and return.
		std::cerr << "ERROR: clearDataBackup : errno = " << errno << '\n';
	}

	// return success
	return rtn;
}

int UbloxGps::ConfigureMessageDataStream(uint8_t classId, uint8_t messageId, bool uart1, bool usb)
{
	// create a msg for sending. 
#pragma pack(push, 1)
	struct
	{
		uint8_t syncChar1;      // sync bytes
		uint8_t syncChar2;      
		uint8_t classId;        // class id byte
		uint8_t messageId;      // message id byte	
		uint8_t messageLength;  // message length
		uint8_t reserved1;      
		uint8_t nmeaClassId;	// desired nmea class id
		uint8_t nmeaMsgId;		// desired nmea message id
		uint8_t i2cPort;		// the I2C port off
		uint8_t uart1Port;		// the uart port to user specified
		uint8_t uart2Port;		// the uart2 port off
		uint8_t usbPort;		// the usb port to user specified
		uint8_t spiPort;		// the spi port off
		uint8_t reserved2;      // reserved
		uint8_t checksumByte1;	// checksum byte 1
		uint8_t checksumByte2;	// checksum byte 2
	} msg{};
#pragma pack(pop)

	msg.syncChar1		= Ublox::UBX::Header::SyncChar1;	
	msg.syncChar2		= Ublox::UBX::Header::SyncChar2;
	msg.classId			= Ublox::UBX::CFG::classId;		
	msg.messageId		= Ublox::UBX::CFG::MSG::messageId;	
	msg.messageLength	= 0x08;						
	msg.reserved1		= 0x00;
	msg.nmeaClassId		= classId;					
	msg.nmeaMsgId		= messageId;				
	msg.i2cPort			= 0x00;						
	msg.uart1Port		= uart1;					
	msg.uart2Port		= 0x00;						
	msg.usbPort			= usb;						
	msg.spiPort			= 0x00;						
	msg.reserved2		= 0x00;						
	msg.checksumByte1	= 0x00;						
	msg.checksumByte2	= 0x00;						

	// Cast each byte of 'msg' to std::byte and construct a pointer to it
	uint8_t* bufferPointer = reinterpret_cast<uint8_t*>(&msg);
	SetUbxChecksum(bufferPointer);

	// write buffer to the ublox device
	if (m_comms.Write(reinterpret_cast<std::byte*>(&msg), sizeof(msg)) < 0)
	{
		// if error, notify and return.
		std::cerr << "ERROR: configureMessageDataStream : errno = " << errno << '\n';
		return -1;
	}

	// return sucess
	return 0;
}

int UbloxGps::ConfigureMessageRate(uint8_t classId, uint8_t messageId, uint8_t rate)
{
	// create a msg for sending. 
#pragma pack(push, 1)
	struct
	{
		uint8_t syncChar1;      // sync bytes
		uint8_t syncChar2;
		uint8_t msgClassId;     // class id byte
		uint8_t msgMessageId;   // message id byte	
		uint8_t messageLength;  // message length
		uint8_t reserved1;
		uint8_t classId;		//
		uint8_t msgId;			//
		uint8_t rate;			//
		uint8_t checksumByte1;	// checksum byte 1
		uint8_t checksumByte2;	// checksum byte 2
	} msg{};
#pragma pack(pop)

	msg.syncChar1			= Ublox::UBX::Header::SyncChar1;
	msg.syncChar2			= Ublox::UBX::Header::SyncChar2;
	msg.msgClassId			= Ublox::UBX::CFG::classId;
	msg.msgMessageId		= Ublox::UBX::CFG::MSG::messageId;
	msg.messageLength		= 0x03;
	msg.reserved1			= 0x00;
	msg.classId				= classId;
	msg.msgId				= messageId;
	msg.rate				= rate;
	msg.checksumByte1		= 0x00;
	msg.checksumByte2		= 0x00;

	// Cast each byte of 'msg' to std::byte and construct a pointer to it
	uint8_t* bufferPointer = reinterpret_cast<uint8_t*>(&msg);
	SetUbxChecksum(bufferPointer);

	int rtn = m_comms.Write(reinterpret_cast<std::byte*>(&msg), sizeof(msg));

	// write buffer to the ublox device
	if (rtn < 0)
	{
		// if error, notify and return.
		std::cerr << "ERROR: configureMessageRate: errno = " << errno << '\n';
		return -1;
	}

	// return success
	return rtn;
}

int UbloxGps::ConfigureDynamics(Ublox::DYNAMICS dynamics)
{
	static const uint32_t key = 0x20110021;
#pragma pack(push, 1)
	struct
	{
		uint8_t		sync1;
		uint8_t		sync2;
		uint8_t		classID;
		uint8_t		messageID;
		uint16_t	payloadLength;
		uint8_t		version;
		uint8_t		layers;
		uint8_t		transaction;
		uint8_t		reserved;
		uint32_t	key;
		uint8_t		value;
		uint16_t	checksum;
	} msg{};
#pragma pack(pop)

	msg.sync1			= Ublox::UBX::Header::SyncChar1;	
	msg.sync2			= Ublox::UBX::Header::SyncChar2;
	msg.classID			= Ublox::UBX::CFG::classId;			
	msg.messageID		= Ublox::UBX::CFG::VALSET::messageId;
	msg.payloadLength	= 0x09;				
	msg.version			= 0x00;				
	msg.layers			= 0x01;				
	msg.transaction		= 0x00;				
	msg.reserved		= 0x00;				
	msg.key				= key;				
	msg.value			= dynamics;			
	
	// Cast each byte of 'msg' to std::byte and construct a pointer to it
	uint8_t* bufferPointer = reinterpret_cast<uint8_t*>(&msg);
	SetUbxChecksum(bufferPointer);

	// write buffer to the ublox device
	if (m_comms.Write(reinterpret_cast<std::byte*>(&msg), sizeof(msg)) < 0)
	{
		// if error, notify and return.
		std::cerr << "ERROR: configureDynamics : errno = " << errno << '\n';
		return -1;
	}

	// return success
	return 0;
}

int UbloxGps::ConfigureDevicePort(uint8_t portId, Ublox::BAUDRATE baud)
{
#pragma pack(push, 1)
	struct
	{
		uint8_t		sync1;
		uint8_t		sync2;
		uint8_t		classID;
		uint8_t		messageID;
		uint16_t	payloadLength;
		uint8_t		portID;
		uint8_t		reserved1;
		int16_t		txReady;
		int32_t		mode;
		uint32_t	baudRate;
		int16_t		inProtoMask;
		int16_t		outProtoMask;
		int16_t		flags;
		uint8_t		reserved2[2];
		uint16_t	checksum;
	} msg{};
#pragma pack(pop)

	uint8_t charLen				= 0b11000000;						// 8bit = 11 for ublox
	uint8_t stopBits_and_parity = 0b0001000;						// 1 stopit = 00 in for ublox and noparity = 100 for ublox 
	uint16_t modeHolder = charLen | (stopBits_and_parity << 8);

	msg.sync1			= Ublox::UBX::Header::SyncChar1;			// set sync bytes
	msg.sync2			= Ublox::UBX::Header::SyncChar2;
	msg.classID			= Ublox::UBX::CFG::classId;					// set class id byte
	msg.messageID		= Ublox::UBX::CFG::PRT::messageId;			// set message id byte			
	msg.payloadLength	= Ublox::UBX::CFG::PRT::payloadSize;		// set message payload length
	msg.portID			= portId;
	msg.reserved1		= 0x00;
	msg.txReady			= 0x00;
	msg.mode			= modeHolder;
	msg.baudRate		= static_cast<uint32_t>(baud);
	msg.inProtoMask		= 0b00100011;
	msg.outProtoMask	= 0b00100011;
	msg.flags			= 0x00;
	msg.reserved2[0]	= 0x00;
	msg.reserved2[1]	= 0x00;

	// Cast each byte of 'msg' to std::byte and construct a pointer to it
	uint8_t* bufferPointer = reinterpret_cast<uint8_t*>(&msg);
	SetUbxChecksum(bufferPointer);

	// write buffer to the ublox device
	if (m_comms.Write(reinterpret_cast<std::byte*>(&msg), sizeof(msg)) < 0)
	{
		// if error, notify and return.
		std::cerr << "ERROR: configureUartPort : errno = " << errno << '\n';
		return -1;
	}

	// return sucess
	return 0;
}

int UbloxGps::SetUbxMessageRate(uint8_t satelliteSource, uint8_t measurmentRate, uint8_t navigationRate)
{
	// create a msg for sending. 
#pragma pack(push, 1)
	struct
	{
		uint8_t syncChar1;      // sync bytes
		uint8_t syncChar2;
		uint8_t classId;        // class id byte
		uint8_t messageId;      // message id byte	
		uint8_t messageLength;  // message length
		uint8_t reserved1;
		uint8_t measRate;		// measurement rate
		uint8_t measRate2;		// measurement rate
		uint8_t navRate;		// desired navigation rate
		uint8_t reserved2;
		uint8_t satSource;		// desired satellite source
		uint8_t reserved3;      // reserved
		uint8_t checksumByte1;	// checksum byte 1
		uint8_t checksumByte2;	// checksum byte 2
	} msg{};
#pragma pack(pop)

	msg.syncChar1			= Ublox::UBX::Header::SyncChar1;
	msg.syncChar2			= Ublox::UBX::Header::SyncChar2;
	msg.classId				= Ublox::UBX::CFG::classId;
	msg.messageId			= Ublox::UBX::CFG::MSG::messageId;
	msg.messageLength		= 0x06;
	msg.reserved1			= 0x00;
	msg.measRate			= measurmentRate;

	// set second byte based on size of first byte
	measurmentRate	== Ublox::UBX::CFG::RATE::MEASURE_HZ1 ? msg.measRate2 = 0x03 : msg.measRate2 = 0x00;

	msg.navRate				= navigationRate;
	msg.reserved2			= 0x00;
	msg.satSource			= satelliteSource;
	msg.reserved3			= 0x00;
	msg.checksumByte1		= 0x00;
	msg.checksumByte2		= 0x00;
	
	// Cast each byte of 'msg' to std::byte and construct a pointer to it
	uint8_t* bufferPointer = reinterpret_cast<uint8_t*>(&msg);
	SetUbxChecksum(bufferPointer);

	// write buffer to the ublox device
	if (m_comms.Write(reinterpret_cast<std::byte*>(&msg), sizeof(msg)) < 0)
	{
		// if error, notify and return.
		std::cerr << "ERROR: setUbxMessageRate : errno = " << errno << '\n';
		return -1;
	}

	// return sucess
	return 0;
}

int UbloxGps::RequestUbxData(uint8_t classId, uint8_t messageId)
{
	// create a msg for sending. 
#pragma pack(push, 1)
	struct
	{
		uint8_t syncChar1;      // set sync bytes
		uint8_t syncChar2;
		uint8_t classId;        // set class id byte
		uint8_t messageId;      // set message id byte	
		uint8_t messageLength;  // set message length
		uint8_t reserved1;
		uint8_t checksumByte1;	// checksum byte 1
		uint8_t checksumByte2;	// checksum byte 2
	} msg{};
#pragma pack(pop)

	msg.syncChar1			= Ublox::UBX::Header::SyncChar1;
	msg.syncChar2			= Ublox::UBX::Header::SyncChar2;
	msg.classId				= classId;		
	msg.messageId			= messageId;	
	msg.messageLength		= 0x00;			
	msg.reserved1			= 0x00;			
	msg.checksumByte1		= 0x00;			
	msg.checksumByte2		= 0x00;
	
	// Cast each byte of 'msg' to std::byte and construct a pointer to it
	uint8_t* bufferPointer = reinterpret_cast<uint8_t*>(&msg);
	SetUbxChecksum(bufferPointer);

	// write the data request buffer to the gps unit.
	int numSent = m_comms.Write(reinterpret_cast<std::byte*>(&msg), sizeof(msg));

	if (numSent < 0)
	{
		// if error, notify and return. 
		std::cerr << "ERROR: requestUbxData : errno = " << errno << '\n';
		return -1;
	}

	return numSent;
}

void UbloxGps::Initialize()
{
	// Flush any old data sitting on serial port
	m_comms.Flush();

	// Next configure the device for no NMEA messages - UBX messages instead
	if (Configure() < 0)
	{
		m_logger.AddLog(m_name, LogClient::LogLevel::Error, "Failed to configure.");
		m_initialized = false;
		return;
	}

	m_initialized = true;
	m_logger.AddLog(m_name, LogClient::LogLevel::Info, "Initialized.");
}

int UbloxGps::Configure()
{
	Ublox::MESSAGE_OUTPUT_RATE desiredMessageRate = Ublox::MESSAGE_OUTPUT_RATE::ONE;
		 if (m_data.SoftwareVersion == Ublox::SW_VERSION::ROM_SPG_5_10)  desiredMessageRate = Ublox::MESSAGE_OUTPUT_RATE::FIVE;
	else if (m_data.SoftwareVersion == Ublox::SW_VERSION::EXT_CORE_4_04) desiredMessageRate = Ublox::MESSAGE_OUTPUT_RATE::FIVE;
	else if (m_data.SoftwareVersion == Ublox::SW_VERSION::EXT_CORE_3_01) desiredMessageRate = Ublox::MESSAGE_OUTPUT_RATE::ONE;

	// Clear the data storages and request an immediate cold start
	if (RestartDevice(Ublox::START_TYPE::COLD, Ublox::RESET_TYPE::HW_RESET_IMMEDIATE) < 0)
	{
		m_logger.AddLog(m_name, LogClient::LogLevel::Error, "Cold start request failed");
		return -1;
	}

	// wait some time for the unit to accept clear command and process
	std::this_thread::sleep_for(std::chrono::milliseconds(2000));

	// turn off NMEA GNGLL - return -1 on error
	if (ConfigureMessageDataStream(Ublox::NMEA::classId, Ublox::NMEA::GxGLL::messageId, false, false) < 0)
	{
		m_logger.AddLog(m_name, LogClient::LogLevel::Error, "Turning off GNGLL failed");
		return -1;
	}
	std::this_thread::sleep_for(std::chrono::milliseconds(1));

	// turn off NMEA GBGSV, GAGSV, GLGSV, GPGSV - return -1 on error
	if (ConfigureMessageDataStream(Ublox::NMEA::classId, Ublox::NMEA::GxGSV::messageId, false, false) < 0)
	{
		m_logger.AddLog(m_name, LogClient::LogLevel::Error, "Turning off GxGSV failed");
		return -1;
	}
	std::this_thread::sleep_for(std::chrono::milliseconds(1));

	// turn off NMEA GNGST - return -1 on error
	if (ConfigureMessageDataStream(Ublox::NMEA::classId, Ublox::NMEA::GxGST::messageId, false, false) < 0)
	{
		m_logger.AddLog(m_name, LogClient::LogLevel::Error, "Turning off GNGST failed");
		return -1;
	}
	std::this_thread::sleep_for(std::chrono::milliseconds(1));

	// turn off NMEA GNGSA - return -1 on error
	if (ConfigureMessageDataStream(Ublox::NMEA::classId, Ublox::NMEA::GxGSA::messageId, false, false) < 0)
	{
		m_logger.AddLog(m_name, LogClient::LogLevel::Error, "Turning off GNGSA failed");
		return -1;
	}
	std::this_thread::sleep_for(std::chrono::milliseconds(1));

	// turn off NMEA GNGGA - return -1 on error
	if (ConfigureMessageDataStream(Ublox::NMEA::classId, Ublox::NMEA::GxGGA::messageId, false, false) < 0)
	{
		m_logger.AddLog(m_name, LogClient::LogLevel::Error, "Turning off GNGGA failed");
		return -1;
	}

	// turn off NMEA GNVTG - return -1 on error
	if (ConfigureMessageDataStream(Ublox::NMEA::classId, Ublox::NMEA::GxVTG::messageId, false, false) < 0)
	{
		m_logger.AddLog(m_name, LogClient::LogLevel::Error, "Turning off GNVTG failed");
		return -1;
	}
	std::this_thread::sleep_for(std::chrono::milliseconds(1));

	// turn off NMEA GNRMC - return -1 on error
	if (ConfigureMessageDataStream(Ublox::NMEA::classId, Ublox::NMEA::GxRMC::messageId, false, false) < 0)
	{
		m_logger.AddLog(m_name, LogClient::LogLevel::Error, "Turning off GNRMC failed");
		return -1;
	}
	std::this_thread::sleep_for(std::chrono::milliseconds(1));

	// turn on UBX NAV DOP - return -1 on error
	if (ConfigureMessageDataStream(Ublox::UBX::NAV::classId, Ublox::UBX::NAV::DOP::messageId, m_commsOnUart, m_commsOnUsb) < 0)
	{
		m_logger.AddLog(m_name, LogClient::LogLevel::Error, "Turning on UBX-NAV-DOP failed");
		return -1;
	}
	std::this_thread::sleep_for(std::chrono::milliseconds(1));

	// set ratefor UBX NAV DOP - return -1 on error
	if (ConfigureMessageRate(Ublox::UBX::NAV::classId, Ublox::UBX::NAV::DOP::messageId, static_cast<uint8_t>(desiredMessageRate)) < 0)
	{
		m_logger.AddLog(m_name, LogClient::LogLevel::Error, "Setting UBX-NAV-DOP failed");
		return -1;
	}
	std::this_thread::sleep_for(std::chrono::milliseconds(1));

	// turn on UBX NAV COV - return -1 on error
	if (ConfigureMessageDataStream(Ublox::UBX::NAV::classId, Ublox::UBX::NAV::COV::messageId, m_commsOnUart, m_commsOnUsb) < 0)
	{
		m_logger.AddLog(m_name, LogClient::LogLevel::Error, "Turning on UBX-NAV-COV failed");
		return -1;
	}
	std::this_thread::sleep_for(std::chrono::milliseconds(1));

	// set rate for UBX NAV COV - return -1 on error
	if (ConfigureMessageRate(Ublox::UBX::NAV::classId, Ublox::UBX::NAV::COV::messageId, static_cast<uint8_t>(desiredMessageRate)) < 0)
	{
		m_logger.AddLog(m_name, LogClient::LogLevel::Error, "Setting UBX-NAV-COV failed");
		return -1;
	}
	std::this_thread::sleep_for(std::chrono::milliseconds(1));

	// turn on UBX NAV PVT - return -1 on error
	if (ConfigureMessageDataStream(Ublox::UBX::NAV::classId, Ublox::UBX::NAV::PVT::messageId, m_commsOnUart, m_commsOnUsb) < 0)
	{
		m_logger.AddLog(m_name, LogClient::LogLevel::Error, "Turning on UBX-NAV-PVT failed");
		return -1;
	}
	std::this_thread::sleep_for(std::chrono::milliseconds(1));

	// set rate for UBX NAV PVT - return -1 on error
	if (ConfigureMessageRate(Ublox::UBX::NAV::classId, Ublox::UBX::NAV::PVT::messageId, static_cast<uint8_t>(desiredMessageRate)) < 0)
	{
		m_logger.AddLog(m_name, LogClient::LogLevel::Error, "Setting UBX-NAV-PVT failed");
		return -1;
	}
	std::this_thread::sleep_for(std::chrono::milliseconds(1));

	// turn on UBX NAV POSECEF - return -1 on error
	if (ConfigureMessageDataStream(Ublox::UBX::NAV::classId, Ublox::UBX::NAV::POSECEF::messageId, m_commsOnUart, m_commsOnUsb) < 0)
	{
		m_logger.AddLog(m_name, LogClient::LogLevel::Error, "Turning on UBX-NAV-POSECEF failed");
		return -1;
	}
	std::this_thread::sleep_for(std::chrono::milliseconds(1));

	// set rate for UBX NAV POSECEF - return -1 on error
	if (ConfigureMessageRate(Ublox::UBX::NAV::classId, Ublox::UBX::NAV::POSECEF::messageId, static_cast<uint8_t>(desiredMessageRate)) < 0)
	{
		m_logger.AddLog(m_name, LogClient::LogLevel::Error, "Setting UBX-NAV-POSECEF failed");
		return -1;
	}
	std::this_thread::sleep_for(std::chrono::milliseconds(1));

	// turn on UBX NAV POSLLH - return -1 on error
	if (ConfigureMessageDataStream(Ublox::UBX::NAV::classId, Ublox::UBX::NAV::POSLLH::messageId, m_commsOnUart, m_commsOnUsb) < 0)
	{
		m_logger.AddLog(m_name, LogClient::LogLevel::Error, "Turning on UBX-NAV-POSLLH failed");
		return -1;
	}
	std::this_thread::sleep_for(std::chrono::milliseconds(1));

	// set rate for UBX NAV POSLLH - return -1 on error
	if (ConfigureMessageRate(Ublox::UBX::NAV::classId, Ublox::UBX::NAV::POSLLH::messageId, static_cast<uint8_t>(desiredMessageRate)) < 0)
	{
		m_logger.AddLog(m_name, LogClient::LogLevel::Error, "Setting UBX-NAV-POSLLH failed");
		return -1;
	}
	std::this_thread::sleep_for(std::chrono::milliseconds(1));

	// turn on UBX NAV VELECEF - return -1 on error
	if (ConfigureMessageDataStream(Ublox::UBX::NAV::classId, Ublox::UBX::NAV::VELECEF::messageId, m_commsOnUart, m_commsOnUsb) < 0)
	{
		m_logger.AddLog(m_name, LogClient::LogLevel::Error, "Turning on UBX-NAV-VELECEF failed");
		return -1;
	}
	std::this_thread::sleep_for(std::chrono::milliseconds(1));

	// set rate for UBX NAV VELECEF - return -1 on error
	if (ConfigureMessageRate(Ublox::UBX::NAV::classId, Ublox::UBX::NAV::VELECEF::messageId, static_cast<uint8_t>(desiredMessageRate)) < 0)
	{
		m_logger.AddLog(m_name, LogClient::LogLevel::Error, "Setting UBX-NAV-VELECEF failed");
		return -1;
	}
	std::this_thread::sleep_for(std::chrono::milliseconds(1));

	// turn on UBX NAV VELNED - return -1 on error
	if (ConfigureMessageDataStream(Ublox::UBX::NAV::classId, Ublox::UBX::NAV::VELNED::messageId, m_commsOnUart, m_commsOnUsb) < 0)
	{
		m_logger.AddLog(m_name, LogClient::LogLevel::Error, "Turning on UBX-NAV-VELNED failed");
		return -1;
	}
	std::this_thread::sleep_for(std::chrono::milliseconds(1));

	// set rate for UBX NAV VELNED - return -1 on error
	if (ConfigureMessageRate(Ublox::UBX::NAV::classId, Ublox::UBX::NAV::VELNED::messageId, static_cast<uint8_t>(desiredMessageRate)) < 0)
	{
		m_logger.AddLog(m_name, LogClient::LogLevel::Error, "Setting UBX-NAV-VELNED failed");
		return -1;
	}
	std::this_thread::sleep_for(std::chrono::milliseconds(1));

	// turn on UBX NAV TIMEUTC - return -1 on error
	if (ConfigureMessageDataStream(Ublox::UBX::NAV::classId, Ublox::UBX::NAV::TIMEUTC::messageId, m_commsOnUart, m_commsOnUsb) < 0)
	{
		m_logger.AddLog(m_name, LogClient::LogLevel::Error, "Turning on UBX-NAV-TIMEUTC failed");
		return -1;
	}
	std::this_thread::sleep_for(std::chrono::milliseconds(1));

	// set rate for UBX NAV TIMEUTC - return -1 on error
	if (ConfigureMessageRate(Ublox::UBX::NAV::classId, Ublox::UBX::NAV::TIMEUTC::messageId, static_cast<uint8_t>(desiredMessageRate)) < 0)
	{
		m_logger.AddLog(m_name, LogClient::LogLevel::Error, "Setting UBX-NAV-TIMEUTC failed");
		return -1;
	}
	std::this_thread::sleep_for(std::chrono::milliseconds(1));

	// turn on UBX NAV SAT - return -1 on error
	if (ConfigureMessageDataStream(Ublox::UBX::NAV::classId, Ublox::UBX::NAV::SAT::messageId, m_commsOnUart, m_commsOnUsb) < 0)
	{
		m_logger.AddLog(m_name, LogClient::LogLevel::Error, "Turning on UBX-NAV-SAT failed");
		return -1;
	}
	std::this_thread::sleep_for(std::chrono::milliseconds(1));

	// set rate for UBX NAV SAT - return -1 on error
	if (ConfigureMessageRate(Ublox::UBX::NAV::classId, Ublox::UBX::NAV::SAT::messageId, static_cast<uint8_t>(desiredMessageRate)) < 0)
	{
		m_logger.AddLog(m_name, LogClient::LogLevel::Error, "Setting UBX-NAV-SAT failed");
		return -1;
	}
	std::this_thread::sleep_for(std::chrono::milliseconds(1));

	// turn on UBX NAV TIMEGPS - return -1 on error
	if (ConfigureMessageDataStream(Ublox::UBX::NAV::classId, Ublox::UBX::NAV::TIMEGPS::messageId, m_commsOnUart, m_commsOnUsb) < 0)
	{
		m_logger.AddLog(m_name, LogClient::LogLevel::Error, "Turning on UBX-NAV-TIMEGPS failed");
		return -1;
	}
	std::this_thread::sleep_for(std::chrono::milliseconds(1));

	// set rate for UBX NAV TIMEGPS - return -1 on error
	if (ConfigureMessageRate(Ublox::UBX::NAV::classId, Ublox::UBX::NAV::TIMEGPS::messageId, static_cast<uint8_t>(desiredMessageRate)) < 0)
	{
		m_logger.AddLog(m_name, LogClient::LogLevel::Error, "Setting UBX-NAV-TIMEGPS failed");
		return -1;
	}
	std::this_thread::sleep_for(std::chrono::milliseconds(1));

	// set dynamic model
	if (ConfigureDynamics(Ublox::DYNAMICS::AIRBORNE_LESS_THAN_1G))
	{
		m_logger.AddLog(m_name, LogClient::LogLevel::Error, "Set Dynamics Model failed");
		return -1;
	}
	std::this_thread::sleep_for(std::chrono::milliseconds(1));

	// return success
	return 0;
}

void UbloxGps::HandleUbxMessage(uint8_t* buffer)
{
	// grab the msg id and class id 
	uint8_t classId = buffer[2];
	uint8_t msgId = buffer[3];

	// get the data length from the buffer at index 4
	uint16_t payloadSize = CalculatePayloadLength(buffer[4], buffer[5]);

	// verify the class id
	if (classId == Ublox::UBX::ACK::classId)
	{
		// Check for ACK message
		if (msgId == Ublox::UBX::ACK::ACK::messageId)
		{
			return;
		}
		// Check for NACK massage
		else if (msgId == Ublox::UBX::ACK::NACK::messageId)
		{
			return;
		}
	}
	else if (classId == Ublox::UBX::MON::classId)
	{
		// Check for MON::VER data
		if (msgId == Ublox::UBX::MON::VER::messageId)
		{
			ParseMonitorVersionData(buffer);

			// Convert the char array to a string for checking if its in our version list
			std::string versionString(m_data.monitorVersion.swVersion);

			// Check if any acceptable version is found in ver
			for (const auto& [enumValue, version] : Ublox::SW_VERSION_MAP)
			{
				if (versionString.find(version) != std::string::npos)
				{
					m_data.SoftwareVersion = enumValue;
					break;
				}
			}
		}
	}
	else if (classId == Ublox::UBX::NAV::classId)
	{
		// Check for NAV::SAT data
		if (msgId == Ublox::UBX::NAV::SAT::messageId)
		{
			ParseNavSatelliteData(buffer);
		}
		// check for NAV::POSECEF data
		else if (msgId == Ublox::UBX::NAV::POSECEF::messageId && payloadSize == Ublox::UBX::NAV::POSECEF::payloadLength)
		{
			ParseNavPositionEcefData(buffer);
		}
		// check for NAV::VELECEF data
		else if (msgId == Ublox::UBX::NAV::VELECEF::messageId && payloadSize == Ublox::UBX::NAV::VELECEF::payloadLength)
		{
			ParseNavVelocityEcefData(buffer);
		}
		// check for NAV::PVT data
		else if (msgId == Ublox::UBX::NAV::PVT::messageId && payloadSize == Ublox::UBX::NAV::PVT::payloadLength)
		{
			ParseNavPositionVelocityTimeData(buffer);
		}
		// check for NAV::TIMEUTC data
		else if (msgId == Ublox::UBX::NAV::TIMEUTC::messageId && payloadSize == Ublox::UBX::NAV::TIMEUTC::payloadLength)
		{
			ParseNavTimeUtcData(buffer);
		}
		// check for NAV::STATUS data
		else if (msgId == Ublox::UBX::NAV::STATUS::messageId && payloadSize == Ublox::UBX::NAV::STATUS::payloadLength)
		{
			ParseNavStatusData(buffer);
		}
		// check for NAV::VELNED data
		else if (msgId == Ublox::UBX::NAV::VELNED::messageId && payloadSize == Ublox::UBX::NAV::VELNED::payloadLength)
		{
			ParseNavVelocityNedData(buffer);
		}
		// check for NAV::DOP data
		else if (msgId == Ublox::UBX::NAV::DOP::messageId && payloadSize == Ublox::UBX::NAV::DOP::payloadLength)
		{
			ParseNavDopData(buffer);
		}
		// check for NAV::PVT data
		else if (msgId == Ublox::UBX::NAV::POSLLH::messageId && payloadSize == Ublox::UBX::NAV::POSLLH::payloadLength)
		{
			ParseNavPositionLlhData(buffer);
		}
		// check for NAV::COV data
		else if (msgId == Ublox::UBX::NAV::COV::messageId && payloadSize == Ublox::UBX::NAV::COV::payloadLength)
		{
			ParseNavCovData(buffer);
		}
		// check for NAV::TIMEGPS data
		else if (msgId == Ublox::UBX::NAV::TIMEGPS::messageId && payloadSize == Ublox::UBX::NAV::TIMEGPS::payloadLength)
		{
			ParseNavTimeGPSData(buffer);
		}
	}
}

int UbloxGps::HexCharToInt(char c)
{
	if (c >= '0' && c <= '9')
	{
		return c - '0';
	}
	if (c >= 'A' && c <= 'F')
	{
		return c - 'A' + 10;
	}
	if (c >= 'a' && c <= 'f')
	{
		return c - 'a' + 10;
	}
	return -1;
}

int UbloxGps::HexToInt(char* c)
{
	// int for return
	int value;

	// append the char array to the int
	value = HexCharToInt(c[0]);
	value = value << 4;
	value += HexCharToInt(c[1]);

	// return int
	return value;
}

uint16_t UbloxGps::CalculatePayloadLength(uint8_t uint1, uint8_t uint2)
{
	return static_cast<uint16_t>(256U * uint2 + uint1);
}

template <typename T>
bool UbloxGps::GetBit(T data, int element)
{
	int mask = 1 << element;
	int masked = data & mask;
	return masked >> element;
}

void UbloxGps::SetUbxChecksum(uint8_t* buffer)
{
	// variables for calculating checksum
	uint8_t i, j;
	uint8_t a = 0, b = 0;

	// find the data range.
	j = (buffer[4] + (buffer[5] << 8) + 6);

	// for each element inside the data range, store the size in hex. 
	for (i = 2; i < j; i++)
	{
		a += buffer[i];
		b += a;
	}

	// set the checksum bytes
	buffer[i + 0] = a;
	buffer[i + 1] = b;
}

bool UbloxGps::ValidateUbxSyncBytes(uint8_t syncByte1, uint8_t syncByte2)
{
	// verify sync byte 1 and 2 match the UBX protocol.
	if ((syncByte1 == Ublox::UBX::Header::SyncChar1) && (syncByte2 == Ublox::UBX::Header::SyncChar2))
	{
		// bytes match, return true
		return true;
	}

	// else return false
	return false;
}

bool UbloxGps::ValidateUbxChecksum(uint8_t* buffer, int size)
{
	uint8_t CK_a = buffer[size - 2];
	uint8_t CK_b = buffer[size - 1];

	// variables for calculating checksum
	int payloadSize, calculatedRange;
	uint8_t a = 0, b = 0;

	// get the paylaod size
	payloadSize = CalculatePayloadLength(buffer[4], buffer[5]);

	// check sum calculates (classID, msgId, lenght, and payload) = payload + 4
	calculatedRange = payloadSize + 4;

	// for each element inside the data range, store the size in hex. 
	// +2 because we do not calculate the sync bytes at locations 0 and 1
	for (int i = 0; i < calculatedRange; i++)
	{
		a += buffer[i + 2];
		b += a;
	}

	// verify the checksum bytes
	if (CK_a == a && CK_b == b)
	{
		return true;
	}

	return false;
}

void UbloxGps::GetUbxChecksums(uint8_t* buffer, int& checkSumA, int& checkSumB)
{
	// variables for calculating checksum
	uint8_t a = 0, b = 0;

	// find the data range.
	int j = (buffer[4] + (buffer[5] << 8) + 6);

	// for each element inside the data range, store the size in hex. 
	for (int i = 2; i < j; i++)
	{
		a += buffer[i];
		b += a;
	}

	// set the checksum bytes
	checkSumA = a;
	checkSumB = b;
}

bool UbloxGps::ValidateNmeaChecksum(char* buffer)
{
	char* checksum_str = nullptr; // pointer to hold the checksum position
	unsigned char calculated_checksum = 0; // holds the calculated checksum

	// Find the checksum delimiter '*'
	checksum_str = std::strchr(buffer, '*');
	if (checksum_str != nullptr)
	{
		*checksum_str = '\0'; // Null-terminate the string at the checksum delimiter

		// Calculate the checksum, starting after the '$' character (i = 1)
		for (size_t i = 1; buffer[i] != '\0'; ++i)
		{
			calculated_checksum ^= buffer[i];
		}

		// Convert the checksum string to an integer
		int checksum = std::stoi(checksum_str + 1, nullptr, 16);

		// Compare calculated checksum with the provided checksum
		if (checksum == calculated_checksum)
		{
			return true; // Checksum is valid
		}
	}

	return false; // Checksum is invalid or missing
}

void UbloxGps::HandleNmeaMessage(char* buffer)
{
	int maxSatellitesUsed = 12;
	int numberSatellitesInView = 0;
	int numberSatellitesPrinted = 0;
	int GnssId = 0;
	std::array<char*, 30> field{};

	// validate check sum
	if (ValidateNmeaChecksum(buffer) == 0)
	{
		return;
	}

	// Parse the NMEA message
	ParseNmeaMessage(buffer, field.data(), field.size());

	std::string_view messageType(buffer, 3);
	std::string_view subMessageType(buffer + 3, 3);

	// GPS or GNSS Messages
	if (messageType == "$GP" || messageType == "$GN")
	{
		// Set the GNSS ID
		GnssId = 1;

		// DATUM REFERENCE MESSAGE 
		// Gives  the difference between the current datum and the reference datum.
		if (subMessageType == "DTM")
		{
			// store
			m_data.nmeaData.latitudeOffset = atof(field[3]);									// latitudeInDeg offset
			field[4][0] == 0 ? m_data.nmeaData.latitudeOffsetDirection = '0' : m_data.nmeaData.latitudeOffsetDirection = field[4][0];							// North or south
			m_data.nmeaData.longitudeOffset = atof(field[5]);									// longitudeInDeg offset
			field[6][0] == 0 ? m_data.nmeaData.longitudeOffsetDirection = '0' : m_data.nmeaData.longitudeOffsetDirection = field[6][0];
			m_data.nmeaData.altitudeOffset = atof(field[7]);									// altitude offset
			m_data.nmeaData.datumReference = field[8];											// datum reference code
		}
		// GPS FIX DATA MESSAGES
		else if (subMessageType == "GGA")
		{
			// store
			m_data.nmeaData.gpsFixData.utcTime = atof(field[1]);								// utc time
			m_data.nmeaData.gpsFixData.latitudeInDegrees = atof(field[2]);						// latitudeInDeg in degreees
			field[3][0] == 0 ? m_data.nmeaData.gpsFixData.latitudeDirection = '0' : m_data.nmeaData.gpsFixData.latitudeDirection = field[3][0];
			m_data.nmeaData.gpsFixData.longitudeInDegrees = atof(field[4]);					// longitudeInDeg in degrees
			field[5][0] == 0 ? m_data.nmeaData.gpsFixData.longitudeDirection = '0' : m_data.nmeaData.gpsFixData.longitudeDirection = field[5][0];
			m_data.nmeaData.gpsFixData.positionFixQuality = std::stoi(field[6]);					// position fix quality
			m_data.nmeaData.gpsFixData.numberSatellitesUsed = std::stoi(field[7]);					// num of sats used 0-12
			m_data.nmeaData.gpsFixData.horizontalDOP = atof(field[8]);							// HDOP
			m_data.nmeaData.gpsFixData.altitudeASLinMeters = atof(field[9]);					// alt above sea level
			m_data.nmeaData.gpsFixData.geoidSeperationInMeters = atof(field[11]);				// difference between ellipsoid and mean sea level
			m_data.nmeaData.gpsFixData.differentialAge = atof(field[13]);						// age of the correction
			m_data.nmeaData.gpsFixData.differentialStationId = atof(field[14]);				// station id providing correction
		}
		// latitudeInDeg AND longitudeInDeg W/ TIME OR POSITION FIX AND STATUS MESSAGE
		else if (subMessageType == "GLL")
		{
			// store
			m_data.nmeaData.latitudeInDegrees = atof(field[1]);								// latitudeInDeg in degreees
			field[2][0] == 0 ? m_data.nmeaData.latitudeDirection = '0' : m_data.nmeaData.latitudeDirection = field[2][0];
			m_data.nmeaData.longitudeInDegrees = atof(field[3]);								// longitudeInDeg in degrees
			field[4][0] == 0 ? m_data.nmeaData.longitudeOffsetDirection = '0' : m_data.nmeaData.longitudeOffsetDirection = field[4][0];
			m_data.nmeaData.utcTime = atof(field[5]);											// utc time
			field[6][0] == 0 ? m_data.nmeaData.dataStatus = '0' : m_data.nmeaData.dataStatus = field[6][0];
			field[7][0] == 0 ? m_data.nmeaData.positionModeGps = '0' : m_data.nmeaData.positionModeGps = field[7][0];
		}
		// GNS FIX DATA MESSAGE
		else if (subMessageType == "GNS")
		{
			// store
			m_data.nmeaData.gnssFixData.utcTime = atof(field[1]);								// utc time
			m_data.nmeaData.gnssFixData.latitudeInDegrees = atof(field[2]);					// latitudeInDeg in degreees
			field[3][0] == 0 ? m_data.nmeaData.gnssFixData.latitudeDirection = '0' : m_data.nmeaData.gnssFixData.latitudeDirection = field[3][0];
			m_data.nmeaData.gnssFixData.longitudeInDegrees = atof(field[4]);					// longitudeInDeg in degrees
			field[5][0] == 0 ? m_data.nmeaData.gnssFixData.longitudeDirection = '0' : m_data.nmeaData.gnssFixData.longitudeDirection = field[5][0];
			m_data.nmeaData.gnssFixData.numberSatellitesUsed = std::stoi(field[7]);					// num of sats used 0-12
			m_data.nmeaData.gnssFixData.horizontalDOP = atof(field[8]);						// HDOP
			m_data.nmeaData.gnssFixData.altitudeASLinMeters = atof(field[9]);					// alt above sea level
			m_data.nmeaData.gnssFixData.geoidSeperationInMeters = atof(field[10]);				// difference between ellipsoid and mean sea level
			m_data.nmeaData.gnssFixData.differentialAge = atof(field[11]);						// age of the correction
			m_data.nmeaData.gnssFixData.differentialStationId = atof(field[12]);				// station id providing correction
			field[13][0] == 0 ? m_data.nmeaData.navStatus = '0' : m_data.nmeaData.navStatus = field[13][0];

			field[6][0] == 0 ? m_data.nmeaData.positionModeGps = '0' : m_data.nmeaData.positionModeGps = field[6][0];
			field[6][1] == 0 ? m_data.nmeaData.positionModeGalileo = '0' : m_data.nmeaData.positionModeGalileo = field[6][1];
			field[6][2] == 0 ? m_data.nmeaData.positionModeGlonass = '0' : m_data.nmeaData.positionModeGlonass = field[6][2];
			field[6][3] == 0 ? m_data.nmeaData.positionModeBeidou = '0' : m_data.nmeaData.positionModeBeidou = field[6][3];
		}
		// GNSS RANGE RESIDUALS
		else if (subMessageType == "GRS")
		{
			// store utc time
			m_data.nmeaData.utcTime = atof(field[1]);

			// handle range for each satellite used
			for (int i = 0; i < maxSatellitesUsed; i++)
			{
				UpdateRangeOfActiveNavigationSatellites(atof(field[3 + i]), i);
			}
		}
		// GNSS DOP + ACTIVE SATELLITES
		else if (subMessageType == "GSA")
		{
			// store
			for (int i = 0; i < maxSatellitesUsed; i++)
			{
				UpdateIdOfActiveNavigationSatellites(std::stoi(field[3 + i]), i);

			}
			m_data.nmeaData.positionDOP = (uint16_t)atof(field[8]);							// Position dilution of precision
			m_data.nmeaData.horizontalDOP = (uint16_t)atof(field[8]);							// Horizontal dilution of precision
			m_data.nmeaData.verticalDOP = (uint16_t)atof(field[8]);							// Vertical dilution of precision
		}
		// GNSS PSEUDO RANGE ERROR STATS
		else if (subMessageType == "GST")
		{
			// store data
			m_data.nmeaData.utcTime = atof(field[1]);											// utc time
			m_data.nmeaData.rangeRMS = atof(field[2]);
			m_data.nmeaData.majorAxisStandardDeviationInMeters = atof(field[3]);
			m_data.nmeaData.minorAxisStandardDeviationInMeters = atof(field[4]);
			m_data.nmeaData.orientationInDegrees = atof(field[5]);
			m_data.nmeaData.latitudeStandardDeviationInMeters = atof(field[6]);
			m_data.nmeaData.longitudeStandardDeviationInMeters = atof(field[7]);
			m_data.nmeaData.altitudeStandardDeviatioInMeters = atof(field[8]);
		}
		// GNSS SATELLITES IN VIEW
		else if (subMessageType == "GSV")
		{
			// if first message in series, reset print variable and set num sats in view
			if (std::stoi(field[2]) == 1)
			{
				numberSatellitesInView = std::stoi(field[3]);
				numberSatellitesPrinted = 0;
			}

			// Set the number of satellites in this message
			int numberSatellitesThisMessage = numberSatellitesInView - numberSatellitesPrinted;
			// max is 4 - if more than 4 cap it at 4, else num is good.  
			numberSatellitesThisMessage > 4 ? numberSatellitesThisMessage = 4 : numberSatellitesThisMessage = numberSatellitesThisMessage;

			int numberSatsPrintedThisMessage = 0;

			// Print data for satellites in view in this message
			for (; numberSatsPrintedThisMessage < numberSatellitesThisMessage; numberSatsPrintedThisMessage++)
			{
				UpdateSatelliteData(GnssId, field[4 + (4 * numberSatsPrintedThisMessage)], field[5 + (4 * numberSatsPrintedThisMessage)], field[6 + (4 * numberSatsPrintedThisMessage)], field[7 + (4 * numberSatsPrintedThisMessage)]);
				numberSatellitesPrinted++;
			}
		}
		// RECOMMENDED MINIMUM DATA
		else if (subMessageType == "RMC")
		{
			// store data
			m_data.nmeaData.utcTime = atof(field[1]);
			field[2][0] == 0 ? m_data.nmeaData.dataStatus = m_data.nmeaData.dataStatus : m_data.nmeaData.dataStatus = field[2][0];
			m_data.nmeaData.latitudeInDegrees = atof(field[3]);
			field[4][0] == 0 ? m_data.nmeaData.latitudeDirection = m_data.nmeaData.latitudeDirection : m_data.nmeaData.latitudeDirection = field[4][0];
			m_data.nmeaData.longitudeInDegrees = atof(field[5]);
			field[6][0] == 0 ? m_data.nmeaData.longitudeDirection = m_data.nmeaData.longitudeDirection : m_data.nmeaData.longitudeDirection = field[6][0];
			m_data.nmeaData.speedOverGroundInKnots = atof(field[7]);
			m_data.nmeaData.courseOverGroundInDegrees = atof(field[8]);
			m_data.nmeaData.utcDate = std::stoi(field[9]);
			m_data.nmeaData.magneticVariationInDegrees = atof(field[10]);
			field[11][0] == 0 ? m_data.nmeaData.magneticVariationDirection = m_data.nmeaData.magneticVariationDirection : m_data.nmeaData.magneticVariationDirection = field[11][0];
			field[12][0] == 0 ? m_data.nmeaData.positionModeGps = m_data.nmeaData.positionModeGps : m_data.nmeaData.positionModeGps = field[12][0];
			field[13][0] == 0 ? m_data.nmeaData.navStatus = m_data.nmeaData.navStatus : m_data.nmeaData.navStatus = field[13][0];
		}
		// TEXT TRANSMISSION
		else if (subMessageType == "TXT")
		{
			// store
			m_data.nmeaData.lastTextTransmissionType = DecodeTxtMsgType(field[3]);
			SetLastTextTransmission(std::stoi(field[2]), field[4]);
		}
		// DUAL GROUND/WATER DISTANCE
		else if (subMessageType == "VLW")
		{
			// store
			m_data.nmeaData.totalCumulativeWaterDistanceInNautMiles = atof(field[1]);
			m_data.nmeaData.waterDistanceSinceResetInNautMiles = atof(field[3]);
			m_data.nmeaData.totalCumulativeGroundDistanceInNautMiles = atof(field[5]);
			m_data.nmeaData.groundDistanceSinceResetInNautMiles = atof(field[7]);
		}
		// COURSE OVER GROUND + GROUND SPEED
		else if (subMessageType == "VTG")
		{
			// store
			m_data.nmeaData.courseOverGroundInDegrees = atof(field[1]);
			m_data.nmeaData.magneticCourseOverGroundInDegrees = atof(field[3]);
			m_data.nmeaData.speedOverGroundInKnots = atof(field[5]);
			m_data.nmeaData.speedOverGroundInKMH = atof(field[7]);
			field[9][0] == 0 ? m_data.nmeaData.positionModeGps = '0' : m_data.nmeaData.positionModeGps = field[9][0];
		}
		// TIME AND DATE 
		else if (subMessageType == "ZDA")
		{
			// store
			m_data.nmeaData.utcTime = atof(field[1]);
			m_data.nmeaData.utcDate = std::stoi(field[2]);
			m_data.nmeaData.localTimeZoneHours = std::stoi(field[5]);
			m_data.nmeaData.localTimeZoneMinutes = std::stoi(field[6]);
		}
		else
		{
			// print message id not handled
			m_logger.AddLog(m_name, LogClient::LogLevel::Info, "MESSAGE NOT HANDLED: " + std::string(field[0]));
		}
	}
	// GLONASS Messages
	else if (messageType == "$GL")
	{
		GnssId = 2;

		// GL POLLING MESSAGE OUTBOUND ONLY
		if (subMessageType == "GLQ")
		{
			return;
		}
		// GNSS SATELLITES IN VIEW
		else if (subMessageType == "GSV")
		{
			// if first message in series, reset print variable and set num sats in view
			if (std::stoi(field[2]) == 1)
			{
				numberSatellitesInView = std::stoi(field[3]);
				numberSatellitesPrinted = 0;
			}

			// Set the number of satellites in this message
			int numberSatellitesThisMessage = numberSatellitesInView - numberSatellitesPrinted;
			// max is 4 - if more than 4 cap it at 4, else num is good.  
			numberSatellitesThisMessage > 4 ? numberSatellitesThisMessage = 4 : numberSatellitesThisMessage = numberSatellitesThisMessage;

			int numberSatsPrintedThisMessage = 0;

			// Print data for satellites in view in this message
			for (; numberSatsPrintedThisMessage < numberSatellitesThisMessage; numberSatsPrintedThisMessage++)
			{
				UpdateSatelliteData(GnssId, field[4 + (4 * numberSatsPrintedThisMessage)], field[5 + (4 * numberSatsPrintedThisMessage)], field[6 + (4 * numberSatsPrintedThisMessage)], field[7 + (4 * numberSatsPrintedThisMessage)]);
				numberSatellitesPrinted++;
			}
		}
		else
		{
			// print message id not handled
			m_logger.AddLog(m_name, LogClient::LogLevel::Info, "MESSAGE NOT HANDLED: " + std::string(field[0]));
		}
	}
	// GALILEO Messages
	else if (messageType == "$GA")
	{
		GnssId = 3;

		// GA POLLING MESSAGE OUTBOUND ONLY
		if (subMessageType == "GAQ")
		{
			return;
		}
		// GNSS SATELLITES IN VIEW
		else if (subMessageType == "GSV")
		{
			// if first message in series, reset print variable and set num sats in view
			if (std::stoi(field[2]) == 1)
			{
				numberSatellitesInView = std::stoi(field[3]);
				numberSatellitesPrinted = 0;
			}

			// Set the number of satellites in this message
			int numberSatellitesThisMessage = numberSatellitesInView - numberSatellitesPrinted;
			// max is 4 - if more than 4 cap it at 4, else num is good.  
			numberSatellitesThisMessage > 4 ? numberSatellitesThisMessage = 4 : numberSatellitesThisMessage = numberSatellitesThisMessage;

			int numberSatsPrintedThisMessage = 0;

			// Print data for satellites in view in this message
			for (; numberSatsPrintedThisMessage < numberSatellitesThisMessage; numberSatsPrintedThisMessage++)
			{
				UpdateSatelliteData(GnssId, field[4 + (4 * numberSatsPrintedThisMessage)], field[5 + (4 * numberSatsPrintedThisMessage)], field[6 + (4 * numberSatsPrintedThisMessage)], field[7 + (4 * numberSatsPrintedThisMessage)]);
				numberSatellitesPrinted++;
			}
		}
		else
		{
			// print message id not handled
			m_logger.AddLog(m_name, LogClient::LogLevel::Info, "MESSAGE NOT HANDLED: " + std::string(field[0]));
		}
	}
	// BEIDOU Messages
	else if (messageType == "$GB")
	{
		GnssId = 4;

		// GB POLLING MESSAGE OUTBOUND ONLY
		if (subMessageType == "GBQ")
		{
			return;
		}
		// GNSS SATELLITES IN VIEW
		else if (subMessageType == "GSV")
		{
			// if first message in series, reset print variable and set num sats in view
			if (std::stoi(field[2]) == 1)
			{
				numberSatellitesInView = std::stoi(field[3]);
				numberSatellitesPrinted = 0;
			}

			// Set the number of satellites in this message
			int numberSatellitesThisMessage = numberSatellitesInView - numberSatellitesPrinted;
			// max is 4 - if more than 4 cap it at 4, else num is good.  
			numberSatellitesThisMessage > 4 ? numberSatellitesThisMessage = 4 : numberSatellitesThisMessage = numberSatellitesThisMessage;

			int numberSatsPrintedThisMessage = 0;

			// Print data for satellites in view in this message
			for (; numberSatsPrintedThisMessage < numberSatellitesThisMessage; numberSatsPrintedThisMessage++)
			{
				UpdateSatelliteData(GnssId, field[4 + (4 * numberSatsPrintedThisMessage)], field[5 + (4 * numberSatsPrintedThisMessage)], field[6 + (4 * numberSatsPrintedThisMessage)], field[7 + (4 * numberSatsPrintedThisMessage)]);
				numberSatellitesPrinted++;
			}
		}
		else
		{
			// print message id not handled
			m_logger.AddLog(m_name, LogClient::LogLevel::Info, "MESSAGE NOT HANDLED: " + std::string(field[0]));
		}
	}
}

int UbloxGps::GetNmeaMsgSignalId(std::string buffer)
{
	// temp variable to holds the location of the *
	size_t temp;

	// check if buffer is empty
	if (buffer.empty())
	{
		return -1;
	}
	else
	{
		// get the location of the '*' indicating the check sum
		temp = buffer.find("*");

		// if the '*' is not found, means it has already been removed 
		// return -1 for not found 
		if (temp == std::string::npos)
		{
			return -1;
		}

		// return the char converted to an int. 
		return std::stoi(&buffer[temp - 1]);
	}
}

void UbloxGps::SetLastTextTransmission(int msgNum, std::string text)
{
	// if first message, clear the transmission variable, and set the text
	if (msgNum == 1)
	{
		m_data.nmeaData.lastTextTransmission.clear();											// clear as precaution
		m_data.nmeaData.lastTextTransmission = text;											// set text
	}
	// else we want to append to current text. 
	else
	{
		// append text to the end of 
		m_data.nmeaData.lastTextTransmission.append(" ");										// spacer
		m_data.nmeaData.lastTextTransmission.append(text);										// append msg
	}
}

// Parsers

int UbloxGps::ParseNmeaMessage(char* buffer, char** fields, size_t max_fields)
{
	// iterator for size of inbound buffer
	size_t i = 0;
	fields[i++] = buffer;

	// Iterate through the string and change each comma into a null to terminate the C string.
	// Then store the address of the next location into an array so we can later reference to the start of each field.
	while ((i < max_fields) && (buffer = std::strchr(buffer, ',')) != nullptr)
	{
		*buffer = '\0';
		fields[i++] = ++buffer;
	}

	// return
	return static_cast<int>(i);
}

void UbloxGps::ParseMonitorVersionData(uint8_t* buffer)
{
	// Start at Buffer+6 (sync bytes, ids, payload length are in first 6 bytes)
	std::copy(buffer + 6, buffer + 6 + sizeof(m_data.monitorVersion), reinterpret_cast<uint8_t*>(&m_data.monitorVersion));

	// Increment message count
	m_data.UbxMonVerCount++;
}

void UbloxGps::ParseNavSatelliteData(uint8_t* buffer)
{
	// Start at Buffer+6 (sync bytes, ids, payload length are in first 6 bytes)
	std::copy(buffer + 6, buffer + 6 + sizeof(m_data.satelliteData), reinterpret_cast<uint8_t*>(&m_data.satelliteData));

	// verify message version so we can parse correctly
	if (m_data.satelliteData.version == Ublox::UBX::NAV::SAT::messageVersion)
	{
		int satsInMsg = m_data.satelliteData.numberSvs;
		int satNum = 0;

		m_data.satellites.clear();

		for (int i = 0; i < satsInMsg; i++)
		{
			satNum = 12 * i;

			// Create a temp satellite and grab its data from the buffer
			Ublox::UBX::NAV::SAT::Satellite temp{};
			std::copy(&buffer[14 + satNum], &buffer[14 + satNum] + sizeof(temp), reinterpret_cast<uint8_t*>(&temp));

			// Handle scaling
			temp.prResidual = static_cast<uint16_t>(temp.prResidual * 0.1);

			// push to the vector
			m_data.satellites.push_back(temp);
		}
	}

	// Increment message count
	m_data.UbxSatDataCount++;
}

void UbloxGps::ParseNavPositionVelocityTimeData(uint8_t* buffer)
{
	// Start at Buffer+6 (sync bytes, ids, payload length are in first 6 bytes)
	std::copy(buffer + 6, buffer + 6 + sizeof(m_data.pvtData), reinterpret_cast<uint8_t*>(&m_data.pvtData));

	// Increment message count
	m_data.UbxPvtCount++;
}

void UbloxGps::ParseNavStatusData(uint8_t* buffer)
{
	// Start at Buffer+6 (sync bytes, ids, payload length are in first 6 bytes)
	std::copy(buffer + 6, buffer + 6 + sizeof(m_data.navigationStatusData), reinterpret_cast<uint8_t*>(&m_data.navigationStatusData));

	// Increment message count
	m_data.UbxNavStatusCount++;

	m_data.pvtData.gpsTowInMs = m_data.navigationStatusData.gpsTowInMs;
}

void UbloxGps::ParseNavPositionEcefData(uint8_t* buffer)
{
	// Start at Buffer+6 (sync bytes, ids, payload length are in first 6 bytes)
	std::copy(buffer + 6, buffer + 6 + sizeof(m_data.positionEcefData), reinterpret_cast<uint8_t*>(&m_data.positionEcefData));

	// Increment message count
	m_data.UbxPosEcefCount++;

	m_data.pvtData.gpsTowInMs = m_data.positionEcefData.gpsTowInMs;
}

void UbloxGps::ParseNavPositionLlhData(uint8_t* buffer)
{
	// Start at Buffer+6 (sync bytes, ids, payload length are in first 6 bytes)
	std::copy(buffer + 6, buffer + 6 + sizeof(m_data.positionLlhData), reinterpret_cast<uint8_t*>(&m_data.positionLlhData));

	// Increment message count
	m_data.UbxPosLlhCount++;

	m_data.pvtData.gpsTowInMs = m_data.positionLlhData.gpsTowInMs;
	m_data.pvtData.latitudeInDeg = m_data.positionLlhData.latitude;
	m_data.pvtData.longitudeInDeg = m_data.positionLlhData.longitude;
	m_data.pvtData.haeInMm = m_data.positionLlhData.hae;
	m_data.pvtData.heightMslInMm = m_data.positionLlhData.heightMSL;
	m_data.pvtData.horizontalAccuracyEstInMm = m_data.positionLlhData.horizontalAccuracyEst;
	m_data.pvtData.verticalAccuracyEstInMm = m_data.positionLlhData.verticalAccuracyEst;

}

void UbloxGps::ParseNavVelocityEcefData(uint8_t* buffer)
{
	// Start at Buffer+6 (sync bytes, ids, payload length are in first 6 bytes)
	std::copy(buffer + 6, buffer + 6 + sizeof(m_data.velocityEcefData), reinterpret_cast<uint8_t*>(&m_data.velocityEcefData));

	// Increment message count
	m_data.UbxVelEcefCount++;

	// Update the PVT structure also as its main source of data to pull from. 
	m_data.pvtData.gpsTowInMs = m_data.velocityEcefData.gpsTowInMs;
}

void UbloxGps::ParseNavVelocityNedData(uint8_t* buffer)
{
	// Start at Buffer+6 (sync bytes, ids, payload length are in first 6 bytes)
	std::copy(buffer + 6, buffer + 6 + sizeof(m_data.velocityNedData), reinterpret_cast<uint8_t*>(&m_data.velocityNedData));

	// Increment message count
	m_data.UbxVelNedCount++;

	// Update the PVT structure also as its main source of data to pull from. 
	m_data.pvtData.gpsTowInMs = m_data.velocityNedData.gpsTowInMs;
	m_data.pvtData.velocityNorthInMms = m_data.velocityNedData.velocityNorthInCms		* cm_to_mm;
	m_data.pvtData.velocityEastInMms = m_data.velocityNedData.velocityEastInCms			* cm_to_mm;
	m_data.pvtData.velocityDownInMms = m_data.velocityNedData.velocityDownInCms			* cm_to_mm;
	m_data.pvtData.groundSpeedInMms = m_data.velocityNedData.groundSpeedInCms			* cm_to_mm;
	m_data.pvtData.headingMotionInDeg = m_data.velocityNedData.headingOfMotionInDeg;
	m_data.pvtData.speedAccuracyEstInMms = m_data.velocityNedData.speedAccuracyEstInCms * cm_to_mm;
	m_data.pvtData.headingAccuracyEstInDeg = m_data.velocityNedData.headingAccuracyEstInDeg;
}
 
void UbloxGps::ParseNavTimeUtcData(uint8_t* buffer)
{
	// Start at Buffer+6 (sync bytes, ids, payload length are in first 6 bytes)
	std::copy(buffer + 6, buffer + 6 + sizeof(m_data.timeUtcData), reinterpret_cast<uint8_t*>(&m_data.timeUtcData));

	// Increment message count
	m_data.UbxTimeUtcCount++;

	// Update the PVT structure also as its main source of data to pull from. 
	m_data.pvtData.gpsTowInMs = m_data.timeUtcData.gpsTowInMs;
	m_data.pvtData.nanoSeconds = m_data.timeUtcData.nanoSeconds;
	m_data.pvtData.year = m_data.timeUtcData.year;
	m_data.pvtData.month = m_data.timeUtcData.month;
	m_data.pvtData.day = m_data.timeUtcData.day;
	m_data.pvtData.hour = m_data.timeUtcData.hour;
	m_data.pvtData.min = m_data.timeUtcData.minute;
	m_data.pvtData.sec = m_data.timeUtcData.seconds;
}

void UbloxGps::ParseNavTimeGPSData(uint8_t* buffer)
{
	// Start at Buffer+6 (sync bytes, ids, payload length are in first 6 bytes)
	std::copy(buffer + 6, buffer + 6 + sizeof(m_data.gpsTime), reinterpret_cast<uint8_t*>(&m_data.gpsTime));

	// Increment message count
	m_data.UbxGpsTimeCount++;

	// Update the PVT structure also as its main source of data to pull from. 
	m_data.pvtData.gpsTowInMs = m_data.gpsTime.gpsTowInMs;
}

void UbloxGps::ParseNavDopData(uint8_t* buffer)
{
	// Start at Buffer+6 (sync bytes, ids, payload length are in first 6 bytes)
	std::copy(buffer + 6, buffer + 6 + sizeof(m_data.dopData), reinterpret_cast<uint8_t*>(&m_data.dopData));

	// Increment message count
	m_data.UbxDopCount++;

	// Update the PVT structure also as its main source of data to pull from. 
	m_data.pvtData.gpsTowInMs = m_data.dopData.gpsTowInMs;
}

void UbloxGps::ParseNavCovData(uint8_t* buffer)
{
	// Start at Buffer+6 (sync bytes, ids, payload length are in first 6 bytes)
	std::copy(buffer + 6, buffer + 6 + sizeof(m_data.navCovarianceData), reinterpret_cast<uint8_t*>(&m_data.navCovarianceData));

	// Increment message count
	m_data.UbxNavCovCount++;
}

// Updaters

void UbloxGps::UpdateSatelliteData(const int gnssId, const char* id, const char* elevation, const char* azimuth, const char* signalStrength)
{
	// create a temp satellite instance and set the data to it. 
	Ublox::NMEA::Satellite temp;
	temp.numberID = std::stoi(id);
	temp.elevationInDegrees = std::stoi(elevation);
	temp.azimuthInDegrees = std::stoi(azimuth);
	temp.signalStrength = std::stoi(signalStrength);

	// if signal strength is 0 = not tracking. else = tracking
	signalStrength == "\0" ? temp.tracking = false : temp.tracking = true;

	// use the gnssId to decipher which vector to add satellite to. 
	switch (gnssId)
	{
	case 1:
		UbloxGps::UpdateSatelliteVector(temp, m_data.nmeaData.gpsSatellites);
		break;
	case 2:
		UbloxGps::UpdateSatelliteVector(temp, m_data.nmeaData.glonassSatellites);
		break;
	case 3:
		UbloxGps::UpdateSatelliteVector(temp, m_data.nmeaData.galileoSatellites);
		break;
	case 4:
		UbloxGps::UpdateSatelliteVector(temp, m_data.nmeaData.beidouSatellites);
		break;
	}
}

void UbloxGps::UpdateSatelliteVector(const Ublox::NMEA::Satellite sat, std::vector<Ublox::NMEA::Satellite>& satList)
{
	// if list is empty, push onto the back. 
	if (satList.empty())
	{
		satList.push_back(sat);
	}
	// else - if the temp.id is found, replace. 
	//		  if the temp.id is NOT found, add to the back of the vector.
	else
	{
		std::vector<Ublox::NMEA::Satellite>::iterator it = std::find(satList.begin(), satList.end(), sat);

		// if found, replace.
		if (it != satList.end())
		{
			std::replace(satList.begin(), satList.end(), *it, sat);
		}
		else // push to the back. 
		{
			satList.push_back(sat);
		}
	}
}

void UbloxGps::UpdateIdOfActiveNavigationSatellites(int id, int satelliteNumber)
{
	m_data.nmeaData.activeNavigationSatellites[satelliteNumber].numberID = id;
}

void UbloxGps::UpdateRangeOfActiveNavigationSatellites(double range, int satelliteNumber)
{
	m_data.nmeaData.activeNavigationSatellites[satelliteNumber].range = range;
}

void UbloxGps::UpdateCommonData()
{
	m_commonData.rxCount = m_data.UbxRxCount + m_data.NmeaRxCount;
}

// Decoders

std::string UbloxGps::DecodeSignalId(const int gnssId, const char* signalId)
{
	// store conversion for return
	std::string type;

	// catch empty string = QZSS
	if (gnssId == 0 && std::string(signalId).length() == 0)
	{
		type = "QZSS";
	}
	// else convert to appropriate type
	else
	{
		switch (gnssId)
		{
		case 1:
			switch (std::stoi(signalId))
			{
			case 1: type = "GPS L1C/A"; break;
			case 5: type = "GPS L2 CM"; break;
			case 6: type = "GPS L2 CL "; break;
			}
			break;
		case 2:
			switch (std::stoi(signalId))
			{
			case 1: type = "GLONASS L1 OF*"; break;
			case 3: type = "GLONASS L2 OF"; break;
			}
			break;
		case 3:
			switch (std::stoi(signalId))
			{
			case 2: type = "Galileo E5"; break;
			case 7: type = "Galileo E1"; break;
			}
			break;
		case 4:
			switch (std::stoi(signalId))
			{
			case 1: type = "BeiDou B1I"; break;
			case 3: type = "BeiDou B2I"; break;
			}
			break;
		default: type = "Unknown";
		}
	}

	// return
	return type;
}

std::string UbloxGps::DecodeDataStatus(const char* status)
{
	// holds decoded meaning
	std::string type;

	// Convert status to a string for comparison
	std::string mode(status != nullptr ? status : "");

	// find the chars appropriate meaning.
	if (status == "V") { type = "Data Invalid"; }
	else if (status == "A") { type = "Data Valid"; }
	else { type = "Error"; }

	// return string of decoded type
	return type;
}

std::string UbloxGps::DecodePosMode(const char* posMode) 
{
	// Holds decoded meaning
	std::string type;

	// Convert posMode to a string for comparison
	std::string mode(posMode != nullptr ? posMode : "");

	// Find the chars' appropriate meaning
	if (mode == "N") { type = "No Fix"; }
	else if (mode == "E") { type = "Estimated/Dead Reckoning Fix"; }
	else if (mode == "A") { type = "Autonomous GNSS Fix"; }
	else if (mode == "D") { type = "Differential GNSS Fix"; }
	else if (mode == "F") { type = "RTK Float"; }
	else if (mode == "R") { type = "RTK Fixed"; }
	else { type = "Error"; }

	// Return string of decoded type
	return type;
}

std::string UbloxGps::DecodeNavMode(const char* navMode)
{
	// holds decoded meaning
	std::string type;

	// convert the char to int and find its appropriate meaning.
	switch (std::stoi(navMode))
	{
	case 1: type = "No Fix"; break;
	case 2: type = "2D Fix"; break;
	case 3: type = "3D Fix"; break;
	default: type = "Error";
	}

	// return string
	return type;
}

std::string UbloxGps::DecodeTxtMsgType(const char* msgType)
{
	// holds decoded meaning
	std::string type;

	// convert the char to int and find its appropriate meaning.
	switch (std::stoi(msgType))
	{
	case 00: type = "Error"; break;
	case 01: type = "Warning"; break;
	case 02: type = "Notice"; break;
	case 07: type = "User"; break;
	default: type = "Error";
	}

	// return string
	return type;
}

std::string UbloxGps::DecodeQuality(const char* quality)
{
	// holds decoded meaning
	std::string type;

	// convert the char to int and find its appropriate meaning.
	switch (std::stoi(quality))
	{
	case 0: type = "No Fix"; break;
	case 1: type = "Autonomous GNSS Fix"; break;
	case 2: type = "Differential GNSS Fix"; break;
	case 4: type = "RTK Fixed"; break;
	case 5: type = "RTK Float"; break;
	case 6: type = "Estimated/Dead Reckoning Fix"; break;
	default: type = "Error";
	}

	// return string
	return type;
}

std::string UbloxGps::DecodeOpMode(const char* opMode) 
{
	// Holds decoded mode
	std::string mode;

	// Convert opMode to a string for comparison
	std::string modeStr(opMode != nullptr ? opMode : "");

	// Decode the opMode
	if (modeStr == "M") { mode = "Manual Mode"; }
	else if (modeStr == "A") { mode = "Automatic Mode"; }
	else { mode = "Error"; }

	// Return string
	return mode;
}

std::string UbloxGps::DecodeGnssId(const char* gnssId)
{
	// store conversion for return
	std::string type;

	// catch empty string = QZSS
	if (std::string(gnssId).length() == 0)
	{
		type = "QZSS";
	}
	// else convert to appropriate type
	else
	{
		switch (std::stoi(gnssId))
		{
		case 1: type = "GPS"; break;
		case 2: type = "GLONASS"; break;
		case 3: type = "Galileo"; break;
		case 4: type = "BeiDou"; break;
		default: type = "Error";
		}
	}

	// return converted type.
	return type;
}

std::string UbloxGps::DecodeSatelliteQuality(const int quality)
{
	// holds decoded meaning
	std::string type;

	// convert the char to int and find its appropriate meaning.
	switch (quality)
	{
	case 0: type = "No Signal"; break;
	case 1: type = "Searching Signal"; break;
	case 2: type = "Signal Acquired"; break;
	case 3: type = "Signal Detected But Unusable"; break;
	case 4: type = "RTK Float"; break;
	case 5:
	case 6:
	case 7: type = "Estimated/Dead Reckoning Fix"; break;
	default: type = "Error";
	}

	// return string
	return type;
}

std::string UbloxGps::DecodeSatelliteSignalHealth(const int health)
{
	// holds decoded meaning
	std::string type;

	// convert the char to int and find its appropriate meaning.
	switch (health)
	{
	case 0: type = "Unknown"; break;
	case 1: type = "Healthy"; break;
	case 2: type = "Unhealthy"; break;
	default: type = "Error";
	}

	// return string
	return type;
}

std::string UbloxGps::DecodeSatelliteOrbitSource(const int source)
{
	// holds decoded meaning
	std::string type;

	// convert the char to int and find its appropriate meaning.
	switch (source)
	{
	case 0: type = "No Info Available"; break;
	case 1: type = "Ephemeris Used"; break;
	case 2: type = "Almanac Used"; break;
	case 3: type = "AssistNow Offline Orbit Used"; break;
	case 4: type = "AssistNow Autonomous Orbit Used"; break;
	case 5:
	case 6:
	case 7: type = "Other Orbit Info Used"; break;
	default: type = "Error";
	}

	// return string
	return type;
}

std::string UbloxGps::DecodeNavStatusPowerSaveState(const uint8_t state)
{
	// holds decoded meaning
	std::string type;

	// convert the char to int and find its appropriate meaning.
	switch (state)
	{
	case 0: type = "Acquisition"; break;
	case 1: type = "Tracking"; break;
	case 2: type = "Power Optimized Tracking"; break;
	case 3: type = "Inactive"; break;
	default: type = "Error";
	}

	// return string
	return type;
}

std::string UbloxGps::DecodeNavPvtPowerSaveState(const uint8_t state)
{
	// holds decoded meaning
	std::string type;

	// convert the char to int and find its appropriate meaning.
	switch (state)
	{
	case 0: type = "PSM Not Active"; break;
	case 1: type = "Enabled"; break;
	case 2: type = "Acquisition"; break;
	case 3: type = "Tracking"; break;
	case 4: type = "Power Optimized Tracking"; break;
	case 5: type = "Inactive"; break;
	default: type = "Error";
	}

	// return string
	return type;
}

std::string UbloxGps::DecodeNavPvtCarrierPhaseRangeSolution(const uint8_t status)
{
	// holds decoded meaning
	std::string type;

	// convert the char to int and find its appropriate meaning.
	switch (status)
	{
	case 0: type = "No Carrier PRS Solution"; break;
	case 1: type = "Carrier PRS w/ Floating Ambig"; break;
	case 2: type = "Carrier PRS w/ Fixed Ambig"; break;
	default: type = "Error";
	}

	// return string
	return type;
}

std::string UbloxGps::DecodeNavTimeUtcStandardIdentifier(const uint8_t time)
{
	// holds decoded meaning
	std::string type;

	// convert the char to int and find its appropriate meaning.
	switch (time)
	{
	case 0: type = "Info Not Available"; break;
	case 1: type = "Communications Research Labratory (CRL)"; break;
	case 2: type = "National Institute of Standards and Technology (NIST)"; break;
	case 3: type = "U.S. Naval Observatory (USNO)"; break;
	case 4: type = "International Bureau of Weights and Measures (BIPM)"; break;
	case 5: type = "European Laboratory (tbd)"; break;
	case 6: type = "Former Soviet Union (SU)"; break;
	case 7: type = "National Time Service Center, China (NTSC)"; break;
	case 15: type = "Unknown"; break;
	default: type = "Error";
	}

	// return string
	return type;
}
