#pragma once
#include <string>
#include <optional>
#include <cstdint>
#include <functional>
#include <filesystem>

#ifdef _WIN32
#include <windows.h>
#else
typedef int HANDLE;
constexpr int INVALID_HANDLE_VALUE = -1;
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#endif

class SerialClient
{
public:
	/// @brief enum for acceptable baud rates for the port
	enum class BaudRate
	{
		BAUDRATE_50,		// Posix only.
		BAUDRATE_75,		// Posix only.
		BAUDRATE_110,
		BAUDRATE_134,		// Posix only.
		BAUDRATE_150,		// Posix only.
		BAUDRATE_200,		// Posix only.
		BAUDRATE_300,
		BAUDRATE_600,
		BAUDRATE_1200,
		BAUDRATE_1800,		// Posix only.
		BAUDRATE_2400,
		BAUDRATE_4800,
		BAUDRATE_9600,
		BAUDRATE_14400,		// Windows only. 
		BAUDRATE_19200,
		BAUDRATE_38400,
		BAUDRATE_57600,
		BAUDRATE_76800,		// Posix only.
		BAUDRATE_115200,
		BAUDRATE_128000,	// Windows only.
		BAUDRATE_256000,	// Windows only.
		BAUDRATE_460800,
		BAUDRATE_921600,
		BAUDRATE_CUSTOM,
		BAUDRATE_AUTO,
		BAUDRATE_INVALID,
	};

	/// @brief enum for the byte size option for the port.
	enum class ByteSize
	{
		FIVE,
		SIX,
		SEVEN,
		EIGHT,
		INVALID,
	};

	/// @brief enum for the parity option for the port.
	enum class Parity
	{
		NONE,
		ODD,
		EVEN,
		MARK,
		SPACE,
		INVALID,
	};

	/// @brief enum for the stopbits option for the port
	enum class StopBits
	{
		ONE,
		TWO,
		ONE_FIVE,
		INVALID
	};

	/// @brief Default Constructor
	SerialClient() {}

	/// @brief Constructor that accepts some default parameters.
	/// @param port - [in] - Port to connect on.
	/// @param baud - [in] - Baudrate for the port.
	/// @param bytes - [in] - BytesSize for the port.
	/// @param parity - [in] - Parity for the port.
	/// @param stopbits - [in] - Stopbits for the port.
	/// @param blocking - [in/opt] - Initialize the port to be blocking. Default false.
	/// @param customBaud - [in/opt] - Custom baud only useful if using BAUDRATE_CUSTOM for baud
	SerialClient(const std::string port, const BaudRate baud, const ByteSize bytesize, const Parity parity, 
		const StopBits stopbits, const bool blocking = false, const int customBaud = -1);
    
	/// @brief Default Deconstructor
	~SerialClient();

	/// @brief Initializer that accepts some default parameters if the default constructor is used.
	/// @param port - [in] - Port to connect on.
	/// @param baud - [in] - Baudrate for the port.
	/// @param bytes - [in] - BytesSize for the port.
	/// @param parity - [in] - Parity for the port.
	/// @param stopbits - [in] - Stopbits for the port.
	/// @param blocking - [in/opt] - Configure the port to be a blocking port
	/// @param customBaud - [in/opt] - Custom baud only useful if using BAUDRATE_CUSTOM for baud
	/// @return 0 if successful, -1 if port is already opened
	int Configure(const std::string port, const BaudRate baud, const ByteSize bytesize, const Parity parity,
		const StopBits stopbits, const bool blocking = false, const int customBaud = -1);

	/// @brief Opens a serial connection.
	/// @return true if successful, false if fails
	bool Open();

	/// @brief Attempt to configure the port and open it
	/// @param port - [in] - Port to connect on.
	/// @param baud - [in] - Baudrate for the port.
	/// @param bytes - [in] - BytesSize for the port.
	/// @param parity - [in] - Parity for the port.
	/// @param stopbits - [in] - Stopbits for the port.
	/// @param blocking - [in/opt] - Configure the port to be a blocking port
	/// @param customBaud - [in/opt] - Custom baud only useful if using BAUDRATE_CUSTOM for baud
	/// @return true if successful, false if fails
	bool OpenConfigure(const std::string port, const BaudRate baud, const ByteSize bytesize, const Parity parity,
		const StopBits stopbits, const bool blocking = false, const int customBaud = -1);
	
	/// @brief Reconfigure a port after it has been opened.
	/// @param port - [in] - Port to connect on.
	/// @param baud - [in] - Baudrate for the port.
	/// @param bytes - [in] - BytesSize for the port.
	/// @param parity - [in] - Parity for the port.
	/// @param stopbits - [in] - Stopbits for the port.
	/// @param blocking - [in/opt] - Configure the port to be a blocking port
	/// @param customBaud - [in/opt] - Custom baud only useful if using BAUDRATE_CUSTOM for baud
	/// @return true if successful, false if fails
	bool Reconfigure(const std::string port, const BaudRate baud, const ByteSize bytesize, const Parity parity,
		const StopBits stopbits, const bool blocking = false, const int customBaud = -1);

	/// @brief Flush the input and output of a serial port
	/// @return true if successful, false if fails
	bool Flush();

	/// @brief Flush the input of a serial port
	/// @return true if successful, false if fails
	bool FlushInputBuffer();

	/// @brief Flush the output of a serial port
	/// @return true if successful, false if fails
	bool FlushOutputBuffer();

	/// @brief Read from serial into the passed in buffer.
	/// @param buffer - [out] - Pointer to a buffer to read into
	/// @param size - [in] - Desired size to be read
	/// @return 0+ if successful, -1 if fails
	int Read(std::byte* buffer, size_t size);

	/// @brief Writes a buffer of specified size over a serial port
	/// @param buffer - [in] - Pointer to a buffer to be sent
	/// @param size - [in] - Size of the data to be sent
	/// @return 0+ if successful, -1 if fails
	int Write(const std::byte* buffer, size_t size);

	/// @brief Closes a serial connection
	/// @return true if successful, false if fails
	bool Close();

	/// @brief Get the current port handle
	/// @return if Linux integer of current port, if windows HANDLE of current handle
	HANDLE GetHandle() const { return m_fd; }

	/// @brief Check if port is open.
	/// @return True if open, false if closed.
	bool IsOpen() const { return m_fd != INVALID_HANDLE_VALUE; }

	/// @brief Get the number of bytes available on the serial port to be read.
	/// @return 0+ on success indicating number of available bytes, -1 if fails
	int BytesInQueue();

	/// @brief Set the timeout length when reading
	/// @param timeout - [in] - timeout in seconds
	/// @return true if success, else false
	bool SetTimeout(int timeout);

	/// @brief Get the current timeout length when reading
	/// @return timeout, -1 on error
	int GetTimeout();

	/// @brief Set the port into a blocking mode
	/// @param onoff - [in] - enable or disable blocking mode
	/// @return true if successfully configured, else false
	bool SetBlockingMode(bool onoff);

	/// @brief Find out if the port is in a blocking mode
	/// @return true if configured to block, else false
	bool GetBlockingMode();

	/// @brief Get the current baud rate
	/// @return enum of the current baud rate
	SerialClient::BaudRate GetBaudRate();

protected:

private:
	 /// @brief Set the parity for the port
	 /// @return true if set successfully, else false
	 bool SetParity();

	 /// @brief Sets the baud rate for the port
	 /// @return true if set successfully, else false
	 bool SetBaudRate();

	 /// @brief Sets the byte size for the port
	/// @return true if set successfully, else false
	 bool SetByteSize();

	 /// @brief Sets the stop bits for the port
	/// @return true if set successfully, else false
	 bool SetStopBits();

	 /// @brief Check that the client has been configured
	 /// @return true if all needed items are not invalid, else false
	 bool CheckIfConfigured();

	
	HANDLE			m_fd					= INVALID_HANDLE_VALUE;			//
	size_t			m_bufferSize			= 0;							//
	std::byte*		m_buffer				= {};							//
	std::string		m_port					= "";							//
	BaudRate		m_baudRate				= BaudRate::BAUDRATE_INVALID;	//
	ByteSize		m_byteSize				= ByteSize::INVALID;			//
	Parity			m_parity				= Parity::INVALID;				//
	StopBits		m_stopbits				= StopBits::INVALID;			//
	int				m_timeout				= 0;							//
	bool			m_blocking				= false;						//
	int				m_customBaud			= -1;							// Holds custom baud
	std::function<void(const std::byte*, size_t)> m_dataCallback = nullptr;	//
	std::function<void(int)> m_errorCallback = nullptr;						//
};