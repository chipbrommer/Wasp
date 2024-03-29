#pragma once
/////////////////////////////////////////////////////////////////////////////////
// @file            tcp_client.h
// @brief           Class to implement a tcp client
// @author          Chip Brommer
/////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////
// 
// Includes:
//          name                    reason included
//          ------------------      ------------------------
#ifdef WIN32
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")
#else
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
typedef int SOCKET;
constexpr int INVALID_SOCKET = -1;
#endif
#include <cstdint>					// Standard integer types
#include <map>						// Error enum to strings.
#include <string>					// Strings
#include <regex>					// Regular expression for ip validation
//
#include "log_client.h"             // Log Client
// 
/////////////////////////////////////////////////////////////////////////////////

/// @brief A class to handle multi-platform connections to a TCP server.
class TcpClient
{
public:
	/// @brief Default Constructor
	/// @param logger - [in] - instance of log client
	TcpClient(LogClient& logger);

	/// @brief Constructor to receive an address and port
	/// @param logger - [in] - instance of log client
	/// @param address - [in] - address of the server to send to.
	/// @param port - [in] - port for the server to send to
	TcpClient(LogClient& logger, const std::string address, const uint16_t port);

	/// @brief Default Deconstructor
	~TcpClient();

	/// @brief Configure the client
	/// @param address - [in] - Address of the server
	/// @param port - [in] - Port of the server
	/// @return 0 if successful, -1 if fails.
	int Configure(const std::string address, const uint16_t port);

	/// @brief Connect to TCP server at the set address and port
	/// @return 0 if successful, -1 if fails. 
	int Connect();

	/// @brief Send data to a server
	/// @param buffer - [in] - Buffer to be sent
	/// @param size - [in] - Size to be sent
	/// @return 0+ if successful (number bytes sent), -1 if fails.
	int Send(const char* buffer, const size_t size);

	/// @brief Receive data from a server
	/// @param buffer - [out] - Buffer to place received data into
	/// @param maxSize - [in] - Maximum number of bytes to be read
	/// @return 0+ if successful (number bytes received), -1 if fails.
	int Receive(void* buffer, const size_t maxSize);

	/// @brief Closes a socket
	void Close();

protected:
private:

	/// @brief Validates an IP address is IPv4 or IPv6
	/// @param ip - [in] - IP Address to be validated
	/// @return -1 error, 1 valid ip4, 2 = valid ip6
	int ValidateIP(const std::string& ip);

	/// @brief Validates a port number is between 0-65535
	/// @param port - [in] - Port number to be validated
	/// @return true = valid, false = invalid
	bool ValidatePort(const uint16_t port);

	// Variables
	LogClient&			m_logger;
	std::string			m_address;
	uint16_t			m_port;
	std::string			m_name;
	bool				m_connected;

#ifdef WIN32
	WSADATA				m_wsaData;
#endif

	SOCKET				m_socket;
};