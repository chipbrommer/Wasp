#include "serial_client.h"

SerialClient::SerialClient(const std::string port, const BaudRate baud, const ByteSize bytesize, const Parity parity, 
    const StopBits stopbits, const bool blocking, const int customBaud) :
    SerialClient() 
{
    Configure(port, baud, bytesize, parity, stopbits, blocking, customBaud);
}

SerialClient::~SerialClient()
{
    Close();
}

int SerialClient::Configure(const std::string port, const BaudRate baud, const ByteSize bytesize, const Parity parity, 
    const StopBits stopbits, const bool blocking, const int customBaud)
{
    if (IsOpen()) return false;

    m_port = port;
    m_baudRate = baud;
    m_byteSize = bytesize;
    m_parity = parity;
    m_stopbits = stopbits;
    m_blocking = blocking;

    if (baud == BaudRate::BAUDRATE_CUSTOM) { m_customBaud = customBaud; }

    return true;
}

bool SerialClient::Open()
{
    // Make sure the port is not already open && we have all the needed specifics
    if (IsOpen() || !CheckIfConfigured()) { return false; }

    // Open the serial port
#ifdef _WIN32
    m_fd = CreateFileA(m_port.c_str(), GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
    if (m_fd == INVALID_HANDLE_VALUE) { return false; }
#else
    if (m_blocking)
    {
        m_fd = open(m_port.c_str(), O_RDWR | O_NOCTTY | O_SYNC);
    }
    else
    {
        m_fd = open(m_port.c_str(), O_RDWR | O_NOCTTY | O_NONBLOCK);
    }

    if (m_fd == INVALID_HANDLE_VALUE) { return false; }
    struct termios portConfig;
    if (tcgetattr(m_fd, &portConfig) != 0) { return false; }
    portConfig.c_cflag |= (CREAD | CLOCAL);


    portConfig.c_lflag &= ~(ECHO | // No echo of any kind
        ECHOE |     // Disable echo ERASE as destructive backspace
        ECHOK |     // Disable  echo KILL as line erase
        ECHONL |    // Disable echo '\n'
        ICANON |    // No canonical input
        ISIG |      // No signal generation
        NOFLSH |    // Enable flushing after interrupt, quit, suspend
        TOSTOP);    // Disable send SIGTTOU for background  output

    portConfig.c_iflag &= ~(BRKINT | // No signal interrupt on break
        INLCR |     // Leave NL
        ICRNL |     // Leave CR
        IGNCR);     // "      "

    portConfig.c_iflag |= IGNBRK; // Ignore break
    portConfig.c_oflag &= ~OPOST;
    portConfig.c_cc[VMIN] = m_blocking ? cc_t(1) : 0;
    portConfig.c_cc[VINTR] = _POSIX_VDISABLE;
    portConfig.c_cc[VQUIT] = _POSIX_VDISABLE;
    portConfig.c_cc[VSTART] = _POSIX_VDISABLE;
    portConfig.c_cc[VSTOP] = _POSIX_VDISABLE;
    portConfig.c_cc[VSUSP] = _POSIX_VDISABLE;
    portConfig.c_cc[VTIME] = m_blocking ? 1 : 0;

    if (tcsetattr(m_fd, TCSANOW, &portConfig) != 0) { return false; }
#endif

    // Set the timeout if specified
    if (!SetTimeout(m_timeout)) 
    {
        Close();
        return false;
    }

    // Set the blocking mode
    if (!SetBlockingMode(m_blocking)) 
    {
        Close();
        return false;
    }

    return Reconfigure(m_port, m_baudRate, m_byteSize, m_parity, m_stopbits, m_blocking, m_customBaud);
}

bool SerialClient::OpenConfigure(const std::string port, const BaudRate baud, const ByteSize bytesize, const Parity parity, 
    const StopBits stopbits, const bool blocking, const int customBaud)
{
    if (IsOpen()) { return false; }

    Configure(port, baud, bytesize, parity, stopbits, blocking, customBaud);

    return Open();
}

bool SerialClient::Reconfigure(const std::string port, const BaudRate baud, const ByteSize bytesize, const Parity parity, 
    const StopBits stopbits, const bool blocking, const int customBaud)
{
    if (!IsOpen())      return false;
    if (!SetParity())   return false;
    if (!SetBaudRate()) return false;
    if (!SetStopBits()) return false;
    if (!SetByteSize()) return false;

    return true;
}

bool SerialClient::Flush() 
{
    bool result = true;
#ifdef _WIN32
    if (FlushFileBuffers(m_fd) == 0) { result = false; }
#else
    if (tcflush(m_fd, TCIOFLUSH) != 0) { result = false; }
#endif
    return result;
}

bool SerialClient::FlushInputBuffer() 
{
    bool result = true;
#ifdef _WIN32
    // No equivalent function on Windows, so we'll just flush the entire buffer
    if (PurgeComm(m_fd, PURGE_RXCLEAR) == 0) { result = false; }
#else
    if (tcflush(m_fd, TCIFLUSH) != 0) { result = false; }
#endif
    return result;
}

bool SerialClient::FlushOutputBuffer() 
{
    bool result = true;
#ifdef _WIN32
    // No equivalent function on Windows, so we'll just flush the entire buffer
    if (PurgeComm(m_fd, PURGE_TXCLEAR) == 0) { result = false; }
#else
    if (tcflush(m_fd, TCOFLUSH) != 0) { result = false; }
#endif
    return result;
}

int SerialClient::Read(std::byte* buffer, size_t size)
{
    // Makse sure the port is open
    if (!IsOpen()) return -1;

    // Holds value returned from reading
    int32_t rtn = -1;

    // Read data from the buffer 
#ifdef WIN32
    DWORD numRead = 0;
    rtn = ReadFile(m_fd, &buffer, static_cast<DWORD>(size), &numRead, NULL);

    // ReadFile returns 0 on fail, adjust to -1; else set to number successfully written.
    if (rtn == 0)
    {
        rtn = -1;
    }
    else
    {
        rtn = numRead;
    }
#elif defined __linux__
    rtn = read(m_fd, buffer, size);
#endif

    // Check if read was successful
    if (rtn < 0)
    {
        return -1;
    }

    // Return result
    return rtn;
}

int SerialClient::Write(const std::byte* buffer, size_t size)
{
    // Makse sure the port is open
    if (!IsOpen()) return -1;

    // Holds value returned from writing
    int32_t rtn = -1;

    // Write the msg over serial port that is open. 
#ifdef WIN32
    DWORD numOut = 0;
    rtn = WriteFile(m_fd, buffer, static_cast<DWORD>(size), &numOut, NULL);

    // WriteFile returns 0 on fail, adjust to -1;
    if (rtn == 0) { rtn = -1; }
    else { rtn = numOut; }

#elif defined __linux__
    rtn = write(m_fd, buffer, size);
#endif

    // Check if send was successful
    if (rtn < 0)
    {
        return -1;
    }

    // Return result
    return rtn;
}

bool SerialClient::Close() 
{
#ifdef _WIN32
    if (CloseHandle(m_fd) == 0) { return false; }
#else
    if (close(m_fd) == -1) { return false; }
#endif
    m_fd = INVALID_HANDLE_VALUE;
    return true;
}

int SerialClient::BytesInQueue() 
{
    int bytes = 0;
#ifdef _WIN32
    DWORD errors;
    COMSTAT comStat;
    if (ClearCommError(m_fd, &errors, &comStat) != 0) { bytes = comStat.cbInQue; }
#else
    if (ioctl(m_fd, FIONREAD, &bytes) == -1) { bytes = -1; }
#endif
    return bytes;
}

bool SerialClient::SetTimeout(int timeout_ms) 
{
#ifdef _WIN32
    COMMTIMEOUTS timeouts;
    if (GetCommTimeouts(m_fd, &timeouts) != 0)
    {
        timeouts.ReadTotalTimeoutConstant = timeout_ms;
        timeouts.ReadTotalTimeoutMultiplier = 0;
        timeouts.ReadIntervalTimeout = 0;

        if (SetCommTimeouts(m_fd, &timeouts) == 0) { return false; }
    }
    else return false;
#else
    struct termios options;
    if (tcgetattr(m_fd, &options) == -1) { return false; }
    options.c_cc[VTIME] = timeout_ms / 100;         // Convert timeout to tenths of a second
    options.c_cc[VMIN] = 0;                         // Return immediately
    if (tcsetattr(m_fd, TCSANOW, &options) == -1) { return false; }
#endif
    return true;                                       // Success
}

int SerialClient::GetTimeout() {
    int timeout = 0;
#ifdef _WIN32
    COMMTIMEOUTS timeouts;
    if (GetCommTimeouts(m_fd, &timeouts) != 0) 
    {
        timeout = timeouts.ReadTotalTimeoutConstant;
    }
    else { timeout = -1; }
#else
    struct termios options;
    if (tcgetattr(m_fd, &options) == -1) { timeout = -1; }
    else { timeout = options.c_cc[VTIME] * 100; }   // Timeout in tenths of a second
#endif
    return timeout;
}

bool SerialClient::SetBlockingMode(bool onoff) {
#ifdef _WIN32
    // Not needed on Windows - non-blocking mode is not supported, needs timeout
#else
    int flags = fcntl(m_fd, F_GETFL, 0);
    if (flags == -1) { return false; }              // Error getting current flags
    if (onoff) { flags &= ~O_NONBLOCK; }            // Turn off non-blocking mode
    else { flags |= O_NONBLOCK; }                   // Turn on non-blocking mode

    if (fcntl(m_fd, F_SETFL, flags) == -1) { return false; }
#endif
    return true;
}

bool SerialClient::GetBlockingMode() 
{
#ifdef _WIN32
    // Not supported on Windows
    return true;                                    // Assume blocking mode by default
#else
    int flags = fcntl(m_fd, F_GETFL, 0);
    if (flags == -1) { return true; }               // Assume blocking mode by default
    return !(flags & O_NONBLOCK);                   // Return true if blocking mode is enabled
#endif
}

bool SerialClient::SetParity()
{
    // if the port has been opened
    if (!IsOpen()) return false;

#ifdef WIN32
    DCB serialSettings = { 0 };
    serialSettings.DCBlength = sizeof(serialSettings);
    if (!GetCommState(m_fd, &serialSettings)) { return false; }

    switch (m_parity)
    {
    case Parity::ODD:		serialSettings.Parity = ODDPARITY;      break;
    case Parity::EVEN:		serialSettings.Parity = EVENPARITY;     break;
    case Parity::MARK:		serialSettings.Parity = MARKPARITY;     break;
    case Parity::SPACE:		serialSettings.Parity = SPACEPARITY;    break;
    default:	// Intentional fall through.
    case Parity::NONE:		serialSettings.Parity = NOPARITY;       break;
    }

    if (!SetCommState(m_fd, &serialSettings)) { return false; }
#else
    // retrieve current configuration
    struct termios portConfig = { 0 };
    if (tcgetattr(m_fd, &portConfig) != 0) { return false; }

    switch (m_parity)
    {
    case Parity::SPACE:  return;
    case Parity::MARK:   return;
    case Parity::NONE:
        portConfig.c_cflag &= (~PARENB);
        break;
    case Parity::EVEN:
        portConfig.c_cflag &= (~PARODD);
        portConfig.c_cflag |= PARENB;
        break;
    case Parity::ODD:
        portConfig.c_cflag |= (PARENB | PARODD);
        break;
    default:
        return;
    }

    if (tcsetattr(m_fd, TCSANOW, &portConfig) != 0) { return false; }
#endif

    return true;
}

bool SerialClient::SetBaudRate()
{
    // if the port has been opened
    if (!IsOpen()) return false;

#ifdef WIN32
    DCB serialSettings = { 0 };
    serialSettings.DCBlength = sizeof(serialSettings);
    if (!GetCommState(m_fd, &serialSettings)) { return false; }

    switch (m_baudRate)
    {
    case BaudRate::BAUDRATE_50:		serialSettings.BaudRate = 50;					break;
    case BaudRate::BAUDRATE_75:		serialSettings.BaudRate = 75;					break;
    case BaudRate::BAUDRATE_110:	serialSettings.BaudRate = CBR_110;				break;
    case BaudRate::BAUDRATE_134:	serialSettings.BaudRate = 134;					break;
    case BaudRate::BAUDRATE_150:	serialSettings.BaudRate = 150;					break;
    case BaudRate::BAUDRATE_200:	serialSettings.BaudRate = 200;					break;
    case BaudRate::BAUDRATE_300:	serialSettings.BaudRate = CBR_300;				break;
    case BaudRate::BAUDRATE_600:	serialSettings.BaudRate = CBR_600;				break;
    case BaudRate::BAUDRATE_1200:	serialSettings.BaudRate = CBR_1200;				break;
    case BaudRate::BAUDRATE_2400:	serialSettings.BaudRate = CBR_2400;				break;
    case BaudRate::BAUDRATE_4800:	serialSettings.BaudRate = CBR_4800;				break;
    case BaudRate::BAUDRATE_9600:	serialSettings.BaudRate = CBR_9600;				break;
    case BaudRate::BAUDRATE_14400:	serialSettings.BaudRate = CBR_14400;			break;
    case BaudRate::BAUDRATE_19200:	serialSettings.BaudRate = CBR_19200;			break;
    case BaudRate::BAUDRATE_38400:	serialSettings.BaudRate = CBR_38400;			break;
    case BaudRate::BAUDRATE_57600:	serialSettings.BaudRate = CBR_57600;			break;
    case BaudRate::BAUDRATE_115200: serialSettings.BaudRate = CBR_115200;			break;
    case BaudRate::BAUDRATE_128000: serialSettings.BaudRate = CBR_128000;			break;
    case BaudRate::BAUDRATE_256000: serialSettings.BaudRate = CBR_256000;			break;
    case BaudRate::BAUDRATE_460800: serialSettings.BaudRate = 460800;				break;
    case BaudRate::BAUDRATE_921600: serialSettings.BaudRate = 921600;				break;
    case BaudRate::BAUDRATE_CUSTOM: serialSettings.BaudRate = m_customBaud;		    break;
    default:						serialSettings.BaudRate = CBR_9600;
    }

    if (!SetCommState(m_fd, &serialSettings)) { return false; }
#else
    // take desired baud rate specific action
    speed_t baudrate;

    switch (m_baudRate)
    {
    case BaudRate::BAUDRATE_50:		baudrate = B50;					break;
    case BaudRate::BAUDRATE_75:		baudrate = B75;					break;
    case BaudRate::BAUDRATE_110:	baudrate = B110;				break;
    case BaudRate::BAUDRATE_134:	baudrate = B134;				break;
    case BaudRate::BAUDRATE_150:	baudrate = B150;				break;
    case BaudRate::BAUDRATE_200:	baudrate = B200;				break;
    case BaudRate::BAUDRATE_300:	baudrate = B300;				break;
    case BaudRate::BAUDRATE_600:	baudrate = B600;				break;
    case BaudRate::BAUDRATE_1200:	baudrate = B1200;			    break;
    case BaudRate::BAUDRATE_2400:	baudrate = B2400;			    break;
    case BaudRate::BAUDRATE_4800:	baudrate = B4800;			    break;
    case BaudRate::BAUDRATE_9600:	baudrate = B9600;			    break;
    case BaudRate::BAUDRATE_14400:	m_customBaud = 14400;			break;
    case BaudRate::BAUDRATE_19200:	baudrate = B19200;			    break;
    case BaudRate::BAUDRATE_38400:	baudrate = B38400;			    break;
    case BaudRate::BAUDRATE_57600:	baudrate = B57600;			    break;
    case BaudRate::BAUDRATE_115200: baudrate = B115200;			    break;
    case BaudRate::BAUDRATE_128000: m_baudRate = 128000;			break;
    case BaudRate::BAUDRATE_256000: m_baudRate = 256000;			break;
    case BaudRate::BAUDRATE_460800: baudrate = 460800;				break;
    case BaudRate::BAUDRATE_921600: baudrate = 921600;				break;
    default:						baudrate = B9600;       
    }

// if the selected rate is supported...
//
    if (m_customBaud == -1)
    {
        // retrieve current configuration
        struct termios portConfig = { 0 };
        if (tcgetattr(m_fd, &portConfig) != 0) { return false; }

        // update baud rates
        cfsetispeed(&portConfig, baudrate);
        cfsetospeed(&portConfig, baudrate);

        // configure port
        if (tcsetattr(m_fd, TCSANOW, &portConfig) != 0) { return false; }
    }
    else
    {
        // retrieve current configuration
        struct termios portConfig = { 0 };
        if (tcgetattr(m_fd, &portConfig) != 0) { return false; }

        
        switch (m_parity)
        {
        case Parity::SPACE:  return;
        case Parity::MARK:   return;
        case Parity::NONE:
            portConfig.c_cflag &= (~PARENB);
            break;
        case Parity::EVEN:
            portConfig.c_cflag &= (~PARODD);
            portConfig.c_cflag |= PARENB;
            break;
        case Parity::ODD:
            portConfig.c_cflag |= (PARENB | PARODD);
            break;
        default:
            return;
        }

        portConfig.c_cflag &= ~CBAUD;
        portConfig.c_cflag |= BOTHER;
        portConfig.c_ispeed = static_int<unsigned int>(m_customBaud);
        portConfig.c_ospeed = static_int<unsigned int>(m_customBaud);
        
        if (tcsetattr(m_fd, TCSANOW, &portConfig) != 0) { return false; }
    }
#endif

    return true;
}

SerialClient::BaudRate SerialClient::GetBaudRate()
{
    return m_baudRate;
}

bool SerialClient::SetByteSize()
{
    // if the port has been opened
    if (!IsOpen()) return false;

#ifdef WIN32
    DCB serialSettings = { 0 };
    serialSettings.DCBlength = sizeof(serialSettings);
    if (!GetCommState(m_fd, &serialSettings)) { return false; }

    switch (m_byteSize)
    {
    case ByteSize::FIVE:	serialSettings.ByteSize = 5;    break;
    case ByteSize::SIX:		serialSettings.ByteSize = 6;    break;
    case ByteSize::SEVEN:	serialSettings.ByteSize = 7;    break;
    default:	// Intentional fall through.
    case ByteSize::EIGHT:	serialSettings.ByteSize = 8;    break;
    }

    if (!SetCommState(m_fd, &serialSettings)) { return false; }
#else
    struct termios portConfig = { 0 };
    if (tcgetattr(m_fd, &portConfig) != 0) { return false; }

    switch (m_byteSize)
    {
    case ByteSize::FIVE:
        portConfig.c_cflag &= (~CSIZE);
        portConfig.c_cflag |= CS5;
        break;
    case ByteSize::SIX:
        portConfig.c_cflag &= (~CSIZE);
        portConfig.c_cflag |= CS6;
        break;
    case ByteSize::SEVEN:
        portConfig.c_cflag &= (~CSIZE);
        portConfig.c_cflag |= CS7;
        break;
    case ByteSize::EIGHT:
        portConfig.c_cflag &= (~CSIZE);
        portConfig.c_cflag |= CS8;
        break;
    default:
        return;
    }

    if (tcsetattr(m_fd, TCSANOW, &portConfig) != 0) { return false; }
#endif

    return true;
}

bool SerialClient::SetStopBits()
{
    // if the port has been opened
    if (!IsOpen()) return false;

#ifdef WIN32
    DCB serialSettings = { 0 };
    serialSettings.DCBlength = sizeof(serialSettings);
    if (!GetCommState(m_fd, &serialSettings)) { return false; }

    switch (m_stopbits)
    {
    case StopBits::TWO:			serialSettings.StopBits = TWOSTOPBITS; break;
    case StopBits::ONE_FIVE:;	serialSettings.StopBits = ONE5STOPBITS; break;
    default:	// Intentional fall through.
    case StopBits::ONE:			serialSettings.StopBits = ONESTOPBIT; break;
    }

    if (!SetCommState(m_fd, &serialSettings)) { return false; }
#else
    // retrieve current configuration
    struct termios portConfig = { 0 };
    if (tcgetattr(m_fd, &portConfig) != 0) { return false; }

    switch (m_stopbits)
    {
    case StopBits::TWO:			portConfig.c_cflag |= CSTOPB; break;
    case StopBits::ONE_FIVE:;	return;
    default:	// Intentional fall through.
    case StopBits::ONE:			portConfig.c_cflag &= (~CSTOPB); break;
    }

    if (tcsetattr(m_fd, TCSANOW, &portConfig) != 0) { return false; }
#endif

    return true;
}

bool SerialClient::CheckIfConfigured()
{
    if (
        m_parity != Parity::INVALID                 &&
        m_baudRate != BaudRate::BAUDRATE_INVALID    &&
        m_byteSize != ByteSize::INVALID             &&
        m_stopbits != StopBits::INVALID             &&
        !m_port.empty()
        )
    {
        return true;
    }

    return false;
}