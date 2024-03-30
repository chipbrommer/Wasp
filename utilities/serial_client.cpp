#include "serial_client.h"

SerialClient::SerialClient(const std::string port, const BaudRate baud, const ByteSize bytesize, const Parity parity) :
    SerialClient() 
{
    m_port = port; 
    m_baudRate = baud; 
    m_byteSize = bytesize;
    m_parity = parity;
}

SerialClient::~SerialClient()
{
    Close();
}

int SerialClient::Configure(const std::string port, const BaudRate baud, const ByteSize bytes, const Parity parity, const bool Blocking)
{
    return 0;
}

int SerialClient::Open()
{
    return 0;
}

int SerialClient::OpenConfigure(const std::string port, const BaudRate baud, const ByteSize bytes, const Parity parity, const bool Blocking)
{
    return 0;
}

bool SerialClient::Reconfigure(const std::string port, const BaudRate baud, const ByteSize bytes, const Parity parity, const bool Blocking)
{
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
    return 0;
}

int SerialClient::Write(const std::byte* buffer, size_t size)
{
    return 0;
}

int SerialClient::Start()
{
    return 0;
}

int SerialClient::Close() 
{
#ifdef _WIN32
    if (CloseHandle(m_fd) == 0) { return -1; }
#else
    if (close(m_fd) == -1) { return -1; }
#endif
    m_fd = INVALID_HANDLE_VALUE;
    return 0;
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

int SerialClient::SetTimeout(int timeout_ms) {
#ifdef _WIN32
    COMMTIMEOUTS timeouts;
    if (GetCommTimeouts(m_fd, &timeouts) != 0)
    {
        timeouts.ReadTotalTimeoutConstant = timeout_ms;
        timeouts.ReadTotalTimeoutMultiplier = 0;
        timeouts.ReadIntervalTimeout = 0;

        if (SetCommTimeouts(m_fd, &timeouts) == 0) { return -1; }
    }
    else return -1;
#else
    struct termios options;
    if (tcgetattr(m_fd, &options) == -1) { return -1; }
    options.c_cc[VTIME] = timeout_ms / 100;         // Convert timeout to tenths of a second
    options.c_cc[VMIN] = 0;                         // Return immediately
    if (tcsetattr(m_fd, TCSANOW, &options) == -1) { return -1; }
#endif
    return 0;                                       // Success
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

int SerialClient::SetBlockingMode(bool onoff) {
#ifdef _WIN32
    // Not needed on Windows - non-blocking mode is not supported, needs timeout
#else
    int flags = fcntl(m_fd, F_GETFL, 0);
    if (flags == -1) { return -1; }                 // Error getting current flags
    if (onoff) { flags &= ~O_NONBLOCK; }            // Turn off non-blocking mode
    else { flags |= O_NONBLOCK; }                   // Turn on non-blocking mode

    if (fcntl(m_fd, F_SETFL, flags) == -1) { return -1; }
#endif
    return 0;
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

