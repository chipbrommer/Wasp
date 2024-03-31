#pragma once
/////////////////////////////////////////////////////////////////////////////////
// @file            ublox.h
// @brief           A class for communicating with Ublox GPS modules
// @author          Chip Brommer
/////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////
//
// Includes:
//          name                            reason included
//          ------------------              ------------------------
#include <string>                           // strings
#include <cstdint>							// standard ints
//
#include "gps_type.h"                       // base class
#include "ublox_info.h"                     // gps info
#include "../utilities/constants.h"			// conversions
// 
/////////////////////////////////////////////////////////////////////////////////

/// @brief Holder of Ublox NMEA related data points
struct UbloxNmeaData
{
	Ublox::NMEA::FixData gnssFixData;
	Ublox::NMEA::FixData gpsFixData;

	double		utcTime										= 0;	///< HHMMSS.ss		
	int			utcDate										= 0;	///< DDMMYY
	int			localTimeZoneHours							= 0;	///< HH
	int			localTimeZoneMinutes						= 0;	///< MM
	char		dataStatus									= 'X';
	char		navStatus									= 'X';
	int			navigationMode								= 0;
	int			operationMode								= 0;
	char		positionModeGps								= 'X';
	char		positionModeGalileo							= 'X';
	char		positionModeGlonass							= 'X';
	char		positionModeBeidou							= 'X';
	double		latitudeInDegrees							= 0;
	char		latitudeDirection							= 'X';
	double		latitudeStandardDeviationInMeters			= 0;
	double		longitudeInDegrees							= 0;
	char		longitudeDirection							= 'X';
	double		longitudeStandardDeviationInMeters			= 0;
	double		altitude									= 0;
	double		altitudeStandardDeviatioInMeters			= 0;
	double		altitudeASLinMeters							= 0;
	double		geoidSeperationInMeters						= 0;
	int			positionFixQuality							= 0;
	int			numberSatellitesUsed						= 0;
	double		differentialAge								= 0;
	double		differentialStationId						= 0;
	std::string datumReference								= "";
	double		latitudeOffset								= 0;
	char		latitudeOffsetDirection						= 'X';
	double		longitudeOffset								= 0;
	char		longitudeOffsetDirection					= 'X';
	double		altitudeOffset								= 0;
	double		speedOverGroundInKnots						= 0;
	double		speedOverGroundInKMH						= 0;
	double		courseOverGroundInDegrees					= 0;
	double		magneticCourseOverGroundInDegrees			= 0;
	double		positionDOP									= 0;
	double		horizontalDOP								= 0;
	double		verticalDOP									= 0;
	double		rangeRMS									= 0;
	double		majorAxisStandardDeviationInMeters			= 0;
	double		minorAxisStandardDeviationInMeters			= 0;
	double		orientationInDegrees						= 0;
	double		magneticVariationInDegrees					= 0;
	char		magneticVariationDirection					= 'X';
	double		totalCumulativeWaterDistanceInNautMiles		= 0;
	double		waterDistanceSinceResetInNautMiles			= 0;
	double		totalCumulativeGroundDistanceInNautMiles	= 0;
	double		groundDistanceSinceResetInNautMiles			= 0;
	std::string lastTextTransmissionType					= "";
	std::string lastTextTransmission						= "";

	Ublox::NMEA::Satellite activeNavigationSatellites[12];
	std::vector<Ublox::NMEA::Satellite> gpsSatellites;
	std::vector<Ublox::NMEA::Satellite> galileoSatellites;
	std::vector<Ublox::NMEA::Satellite> glonassSatellites;
	std::vector<Ublox::NMEA::Satellite> beidouSatellites;
};

/// @brief Ublax data storage
struct UbloxData
{
	UbloxNmeaData nmeaData = {};

	Ublox::SW_VERSION								SoftwareVersion = Ublox::SW_VERSION::NOT_SUPPORTED;
	bool											messsagesInitialized = false;
	Ublox::UBX::MON::VER::Message					monitorVersion = {};
	std::vector<char[Ublox::EXTENDED_VERSION_DATA_SIZE]> extendedVersion;
	Ublox::UBX::NAV::COV::Message					navCovarianceData = {};
	Ublox::UBX::NAV::DOP::Message					dopData = {};
	Ublox::UBX::NAV::SAT::Message					satelliteData = {};
	Ublox::UBX::NAV::STATUS::Message				navigationStatusData = {};
	Ublox::UBX::NAV::POSECEF::Message				positionEcefData = {};
	Ublox::UBX::NAV::POSLLH::Message				positionLlhData = {};
	Ublox::UBX::NAV::VELECEF::Message				velocityEcefData = {};
	Ublox::UBX::NAV::VELNED::Message				velocityNedData = {};
	Ublox::UBX::NAV::TIMEUTC::Message				timeUtcData = {};
	Ublox::UBX::NAV::PVT::Message					pvtData = {};
	Ublox::UBX::NAV::TIMEGPS::Message				gpsTime = {};
	std::vector<Ublox::UBX::NAV::SAT::Satellite>	satellites;
	std::vector<Ublox::UBX::NAV::ORB::Satellite>	orbits;

	long UbxRxCount			= 0;
	long UbxMonVerCount		= 0;
	long UbxNavCovCount		= 0;
	long UbxDopCount		= 0;
	long UbxSatDataCount	= 0;
	long UbxNavStatusCount	= 0;
	long UbxPosEcefCount	= 0;
	long UbxPosLlhCount		= 0;
	long UbxVelEcefCount	= 0;
	long UbxVelNedCount		= 0;
	long UbxTimeUtcCount	= 0;
	long UbxPvtCount		= 0;
	long UbxGpsTimeCount	= 0;
	long NmeaRxCount		= 0;
	long ChecksumFailCount	= 0;
};

/// @brief 
class UbloxGps : public GpsType
{
public:

    /// @brief Default Constructor
    UbloxGps(LogClient& logger, const std::string path, const SerialClient::BaudRate baudrate);

    /// @brief Default Deconstructor
    ~UbloxGps() {}

    /// @brief Read data from the gps unit and process it
    int ProcessData() override;

    /// @brief Restarts the Ublox receiver.
	/// @param start - [in] - The desired start type ie: hot, warm, cold
	/// @param reset - [in] - The desired reset type
	/// @return -1 on error, 0+ on success
	int	RestartDevice(Ublox::START_TYPE start, Ublox::RESET_TYPE reset);

	GpsData GetCommonData();

protected:

private:
	/// @brief Initialize the ublox interface
	void Initialize();

	/// @brief Configure the ublox device
	/// @return -1 on error, else 0
	int Configure();

	/// @brief Disables or enables a desired message data stream from the uBlox GPS
	/// @param classId - [in] - classId of the message we want to enable
	/// @param messageId  - [in] - messageId of the message we want to enable
	/// @param uart1 - [in] - enter 1 or 0 to disable or enable this port
	/// @param usb - [in] - enter 1 or 0 to disable or enable this port
	/// @return -1 on fail and errno will be set, 0 on success
	int ConfigureMessageDataStream(uint8_t classId, uint8_t messageId, bool uart1, bool usb);

	/// @brief Configures the message rate for a Ublox message
	/// @param classId - [in] - Class ID of the message to be configured
	/// @param messageId - [in] - Message ID of the message to be configured
	/// @param rate - [in] - Rate for the message to be sent. 
	/// @return -1 on error, 0+ on success (num bytes written)
	int ConfigureMessageRate(uint8_t classId, uint8_t messageId, uint8_t rate);

	/// @brief Configure ther dynamics for the ublox device
	/// @param dynamics - [in] - dynamic setting for the device
	/// @return -1 on error, else 0
	int ConfigureDynamics(Ublox::DYNAMICS dynamics);

	/// @brief Configure the port on the ublox device
	/// @param portId - [in] - port id of the port to be configured
	/// @param baud - [in] - baud rate to set for the units port
	/// @return -1 on error, else 0
	int ConfigureDevicePort(uint8_t portId, Ublox::BAUDRATE baud);

	/// @brief  Allows us to change the configured messaging data rates as desired. 
	/// GPS Ex. (Ublox::UBX::CFG::RATE::GPS_SOURCE, Ublox::UBX::CFG::RATE::MEASURE_HZ10, Ublox::UBX::CFG::RATE::NAV_CYCLES2).
	///	This would set GPS satellite measurement data rate at 10hz and satellite data rate at 2 cycles within this 10hz = 5hz.
	/// @param satelliteSource - [in] - Satellite source to configure rate for. Ex. UBX::CFG::RATE::GPS_SOURCE
	/// @param measureRate - [in] - Desired measurement data rate. Ex. 10hz = UBX::CFG::RATE::MEASURE_HZ10
	/// @param navigationCycles - [in] - Desired navigation cycles rate based on desired measure rate. 
	/// @return -1 on error, else the number of bytes successfully sent.
	int SetUbxMessageRate(uint8_t satelliteSource, uint8_t measurmentRate, uint8_t navigationRate);

	//! @brief Sends a message request over the serial connection to the GPS
	//! @param classId - [in] - classId of the message we want to request
	//! @param messageId  - [in] - messageId of the message we want to request
	//! @return -1 on error, else the number of bytes successfully sent. 
	int RequestUbxData(uint8_t classId, uint8_t messageId);

	//! @brief Handles a received UBX message and maps the data accordingly
	//! @param buffer - [in] - buffer containing the received UBX message
	void HandleUbxMessage(uint8_t* buffer);

	/// @brief Converts a single hex char to an integer.
	/// @param c - [in] - the character to be converted
	/// @return the int value of the converted char
	int HexCharToInt(char c);

	/// @brief Converts a char array of hex to integers
	/// @param c - [in] - The array to be converted  
	/// @return the int value of the array. 
	int HexToInt(char* c);

	/// @brief Calculates payload length from two bytes in little endian
	/// @param uint1 - int 1 to be combined
	/// @param uint2 - int 2 to be combined
	/// @return uint16_t combination of the two. 
	uint16_t CalculatePayloadLength(uint8_t uint1, uint8_t uint2);

	/// @brief Gets the desired bit from a variable
	/// @tparam T - template definition for any inbound data type
	/// @param data - data to get a bit from
	/// @param element - bit location to get
	/// @return - value of the bit: 1,0
	template <typename T>
	bool GetBit(T data, int element);

    /// @brief Sets the UBX checksum for a UBX message buffer. 
	/// @param buffer - [in] - Message to set the checksum of. 
	void SetUbxChecksum(uint8_t* buffer);

	/// @brief Validates the sync bytes of a UBX message
	/// @param syncByte1 - [in] - First sync byte to be validated.
	/// @param syncByte2 - [in] - Seconds sync byte to be validated.
	/// @return 1 for good sync byts, 0 for bad.
	bool ValidateUbxSyncBytes(uint8_t syncByte1, uint8_t syncByte2);

	/// @brief Validates the checksum of a UBX message
	/// @param buffer - [in] - Buffer containing the UBX message
	/// @param size - [in] - size of the buffer data to calculate checksum of
	/// @return 1 for good checksum, 0 for bad.
	bool ValidateUbxChecksum(uint8_t* buffer, int size);

	/// @brief Sets the UBX checksum for a UBX message buffer
	/// @param checkSumA - [out] - variable to hold the first checksum
	/// @param checkSumB - [out] - variable to holds the second checksum
	void GetUbxChecksums(uint8_t* buffer, int& checkSumA, int& checkSumB);

	/// @brief Validates the checksum of an NMEA message
	/// @param buffer - [in] - char array containing the NMEA message
	/// @return 1 on good checksum, 0 on fail. 
	bool ValidateNmeaChecksum(char* buffer);

	/// @brief handles a received NMEA message and maps the data based on message type. 
	/// @param buffer - [in] - buffer containing the inbound message
	void HandleNmeaMessage(char* buffer);

	/// @brief Gets the Signal ID of an NMEA message
	/// @param buffer - [in] - the buffer containing the NMEA message 
	/// @return int containing the signal id, or -1 on empty buffer or if * not found.
	int GetNmeaMsgSignalId(std::string buffer);

	/// @brief Sets the last text transmission variable
	/// @param msgNum - [in] - number of message in the sequence
	/// @param text - [in] - string to store the data in. 
	void SetLastTextTransmission(int msgNum, std::string text);

	/// @brief Parses an NMEA message and splits it into an array at every ","
	/// @param buffer - [in] - Buffer containing the NMEA message  
	/// @param fields - [out] - Char array to store the parsed message cuts into
	/// @param max_fields - [in] - maximum number of fields we can parse into.
	/// @return The number of fields parsed into
	int ParseNmeaMessage(char* buffer, char** fields, size_t max_fields);

	/// @brief Parses a UBX::MON::VER message to the appropriate variables.
	/// @param buffer - [in] - buffer to be parsed. 
	void ParseMonitorVersionData(uint8_t* buffer);

	/// @brief Parses a UBX::NAV::SAT message to the appropriate variables.
	/// @param buffer - [in] - buffer to be parsed. 
	void ParseNavSatelliteData(uint8_t* buffer);

	/// @brief Parses a UBX::NAV::PVT message to the appropriate variables.
	/// @param buffer - [in] - buffer to be parsed. 
	void ParseNavPositionVelocityTimeData(uint8_t* buffer);

	/// @brief Parses a UBX::NAV::STATUS message to the appropriate variables.
	/// @param buffer - [in] - buffer to be parsed. 
	void ParseNavStatusData(uint8_t* buffer);

	/// @brief Parses a UBX::NAV::POSECEF message to the appropriate variables.
	/// @param buffer - [in] - buffer to be parsed. 
	void ParseNavPositionEcefData(uint8_t* buffer);

	/// @brief Parses a UBX::NAV::POSLLH message to the appropriate variables.
	/// @param buffer - [in] - buffer to be parsed. 
	void ParseNavPositionLlhData(uint8_t* buffer);

	/// @brief Parses a UBX::NAV::VELECEF message to the appropriate variables.
	/// @param buffer - [in] - buffer to be parsed.  
	void ParseNavVelocityEcefData(uint8_t* buffer);

	/// @brief Parses a UBX::NAV::VELNED message to the appropriate variables.
	/// @param buffer - [in] - buffer to be parsed. 
	void ParseNavVelocityNedData(uint8_t* buffer);

	/// @brief Parses a UBX::NAV::TIMEUTC message to the appropriate variables.
	/// @param buffer - [in] - buffer to be parsed. 
	void ParseNavTimeUtcData(uint8_t* buffer);

	/// @brief Parses a UBX::NAV::TIMEGPS message to the appropriate variables.
	/// @param buffer - [in] - buffer to be parsed. 
	void ParseNavTimeGPSData(uint8_t* buffer);

	/// @brief Parses a UBX::NAV::DOP message to the appropriate variables.
	/// @param buffer - [in] - buffer to be parsed. 
	void ParseNavDopData(uint8_t* buffer);

	/// @brief Parses a UBX::NAV::COV message to the appropriate variables.
	/// the data comes across in little endian and gets converted to decimal
	/// @param buffer - [in] - buffer to be parsed. 
	void ParseNavCovData(uint8_t* buffer);

	/// @brief Will update or add satellite data to the list of satellites per type
	/// @param gnssId - [in] - id of the type of satellite
	/// @param id - [in] - id of the satellite itself
	/// @param elevation - [in] - elevation in degrees
	/// @param azimuth - [in] - azimuth in degrees
	/// @param signalStrength - [in] - signal strength of the satellite connection
	void UpdateSatelliteData(const int gnssId, const char* id, const char* elevation, const char* azimuth, const char* signalStrength);

	/// @brief Updates the data within the satellite list for NMEA messages. If sat.id is not found it adds new sat to list, else it updates the values.
	/// @param sat - [in] - Satellite instance to add or update
	/// @param satList - [in] - Satellite list for the sat instance
	void UpdateSatelliteVector(const Ublox::NMEA::Satellite sat, std::vector<Ublox::NMEA::Satellite>& satList);

	/// @brief Updates the list of Navigation Satellites, id's
	/// @param id - [in] - ID of the satellite. 
	/// @param satelliteNumber - [in] - array element (satellite number) to set the ID for.
	void UpdateIdOfActiveNavigationSatellites(const int id, const int satelliteNumber);

	/// @brief Updates the list of Navigation Satellites, ranges
	/// @param range - [in] - range of the satellite
	/// @param satelliteNumber - [in] - array element (satellite number) to set the range for.
	void UpdateRangeOfActiveNavigationSatellites(const double range, const int satelliteNumber);

	/// @brief Decode the signal ID 
	/// @param gnssId - [in] - GNSS Id 
	/// @param signalId - [in] - Signal ID
	/// @return string containing the satellite signal ID
	std::string DecodeSignalId(const int gnssId, const char* signalId);

	/// @brief Converts the status indicator into its readable type Ex: 'A' = "Data Valid"
	/// @param status - [in] - indicator to be decoded
	/// @return string of the decoded indicator
	std::string DecodeDataStatus(const char* status);

	/// @brief Decode the posMode data into a string 
	/// @param posMode - [in] - data code for the position mode
	/// @return - string containing the decoded value/meaning
	std::string DecodePosMode(const char* posMode);

	/// @brief Decode the navMode data into a string 
	/// @param navMode - [in] - data code for the navigation mode
	/// @return - string containing the decoded value/meaning
	std::string DecodeNavMode(const char* navMode);

	/// @brief Decode the txt data type into a string 
	/// @param msgType - [in] - data code for the text message type
	/// @return - string containing the decoded value/meaning
	std::string DecodeTxtMsgType(const char* msgType);

	/// @brief Decode the quality data type into a string
	/// @param quality - [in] - data code for the gps fix quality 
	/// @return - string containing the decoded value/meaning
	std::string DecodeQuality(const char* quality);

	/// @brief Decode the operation mode into a sting
	/// @param opMode - [in] - data code for the gps operation mode
	/// @return - string containing the decoded value/meaning
	std::string DecodeOpMode(const char* opMode);

	/// @brief Decode the GNSS ID into a string for the type of satellite.
	/// @param gnssId - [in] - data code of the satellite to be decoded. 
	/// @return - string containing the type of satellite.
	std::string DecodeGnssId(const char* gnssId);

	/// @brief Decode the satellite quality into a string
	/// @param quality - [in] - int value of the quality to be decoded
	/// @return - string containing the decoded value/meaning
	std::string DecodeSatelliteQuality(const int quality);

	/// @brief Decode the satellite health into a string
	/// @param health - [in] - int value of the health to be decoded
	/// @return - string containing the decoded value/meaning
	std::string DecodeSatelliteSignalHealth(const int health);

	/// @brief Decode the satellite orbit source into a string
	/// @param source - [in] - int value of the orbit source
	/// @return - string containing the decoded value/meaning
	std::string DecodeSatelliteOrbitSource(const int source);

	/// @brief Decode the navigation power state into a string
	/// @param state - [in] - int value of the power state
	/// @return - string containing the decoded value/meaning
	std::string DecodeNavStatusPowerSaveState(const uint8_t state);

	/// @brief Decode the navigation power state into a string
	/// @param state - [in] - int value of the power state
	/// @return - string containing the decoded value/meaning
	std::string DecodeNavPvtPowerSaveState(const uint8_t state);

	/// @brief Decode the navigation power state into a string
	/// @param state - [in] - int value of the power state
	/// @return - string containing the decoded value/meaning
	std::string DecodeNavPvtCarrierPhaseRangeSolution(const uint8_t status);

	/// @brief Decode the UTC Standard Identifier into a string
	/// @param time - [in] - int value of the utc standard identifier
	/// @return - string containing the decoded value/meaning
	std::string DecodeNavTimeUtcStandardIdentifier(const uint8_t time);

    /// @brief Updates the common data structure
    void UpdateCommonData() override;

    UbloxData m_data = {};              /// Data storage

	const int m_commsOnUsb = 0;
	const int m_commsOnUart = 1;
};