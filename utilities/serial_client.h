
#ifdef __linux__
	
#endif

typedef int HANDLE;

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

	/// @brief enum for the stop bits options for the port.
	enum class StopBits
	{
		ONE,
		TWO,
		ONE_FIVE,
		INVALID
	};

	/// @brief enum for the flow control options for the port.
	enum class FlowControl
	{
		NONE,
		SOFTWARE,
		HARDWARE,
	};

    SerialClient();
	SerialClient(const std::string port, const BaudRate baud, const ByteSize bytes, const Parity parity);
    ~SerialClient();
	int Configure();
	int Open();
	int OpenConfigure(const std::string port, const BaudRate baud, const ByteSize bytes, const Parity parity,
		const bool Blocking = false);
	int Reconfigure();
	int Flush();
	int FlushInputBuffer();
	int FlushOutputBuffer();
	int Read(uint8_t* buffer, size_t size);
	int Write(const uint8_t* buffer, size_t size);
	int Close();
	HANDLE GetHandle();
	bool IsOpen();
	int BytesInQueue();
	int SetTimeout(int timeout);
	int GetTimeout();
	int SetBlockingMode(bool onoff);
	bool GetBlockingMode();

protected:

private:
	HANDLE m_fd;
};