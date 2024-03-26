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
		BAUDRATE_INVALID,
	};

	/// @brief enum for the byte size options for the port.
	enum class ByteSize
	{
		FIVE,
		SIX,
		SEVEN,
		EIGHT,
		INVALID,
	};

	/// @brief enum for the parity options for the port.
	enum class Parity
	{
		NONE,
		ODD,
		EVEN,
		MARK,
		SPACE,
		INVALID,
	};

	SerialClient() {}
	SerialClient(const std::string port, const BaudRate baud, const ByteSize bytes, const Parity parity);
    ~SerialClient();
	int Configure(const std::string port, const BaudRate baud, const ByteSize bytes, const Parity parity,
		const bool Blocking = false);
	int Open();
	int OpenConfigure(const std::string port, const BaudRate baud, const ByteSize bytes, const Parity parity,
		const bool Blocking = false);
	int Reconfigure(const std::string port, const BaudRate baud, const ByteSize bytes, const Parity parity,
		const bool Blocking = false);
	bool Flush();
	bool FlushInputBuffer();
	bool FlushOutputBuffer();
	int Read(std::byte* buffer, size_t size);
	int Write(const std::byte* buffer, size_t size);
	int Start();
	int Close();
	HANDLE GetHandle() const { return m_fd; }
	bool IsOpen() const { return m_fd != INVALID_HANDLE_VALUE; }
	int BytesInQueue();
	int SetTimeout(int timeout);
	int GetTimeout();
	int SetBlockingMode(bool onoff);
	bool GetBlockingMode();

protected:

private:
	HANDLE			m_fd					= INVALID_HANDLE_VALUE;
	size_t			m_bufferSize			= 0;
	std::byte*		m_buffer				= {};
	std::string		m_port					= "";
	BaudRate		m_baudRate				= BaudRate::BAUDRATE_INVALID;
	ByteSize		m_byteSize				= ByteSize::INVALID;
	Parity			m_parity				= Parity::INVALID;
	bool			m_blocking				= false;
	std::function<void(const std::byte*, size_t)> m_dataCallback;
	std::function<void(int)> m_errorCallback;
};