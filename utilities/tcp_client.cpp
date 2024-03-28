
/////////////////////////////////////////////////////////////////////////////////
// @file            tcp_client.cpp
// @brief           Implementation of the TCP client class
// @author          Chip Brommer
/////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////
// 
// Includes:
//          name                    reason included
//          ------------------      ------------------------
#include <string>                   // strings
//
#include "tcp_client.h"             // Log Client
//
/////////////////////////////////////////////////////////////////////////////////

TcpClient::TcpClient(LogClient& logger) : m_logger(logger), m_address(""), 
	m_port(0), m_name("TCP"), m_connected(false), m_socket(INVALID_SOCKET)
{
#ifdef WIN32
	m_wsaData = {};
#endif
}

TcpClient::TcpClient(LogClient& logger, const std::string address, const uint16_t port) : TcpClient(logger)
{
	if (ValidateIP(address) >= 0)
	{
		m_address = address;
	}

	if (ValidatePort(port))
	{
		m_port = port;
	}
}

TcpClient::~TcpClient()
{
	Close();
}

int TcpClient::Configure(const std::string address, const uint16_t port)
{
	if (ValidateIP(address) >= 0)
	{
		m_address = address;
	}
	else
	{
		return -1;
	}

	if (ValidatePort(port) == true)
	{
		m_port = port;
	}
	else
	{
		return -1;
	}

	return 0;
}

int TcpClient::Connect()
{
	if (m_connected)
	{
		return 1;
	}

	if (m_address.empty())
	{
		return -1;
	}

	if (m_port == 0)
	{
		return -1;
	}

#ifdef WIN32
	if (WSAStartup(MAKEWORD(2, 2), &m_wsaData) != 0)
	{
		return -1;
	}

	m_socket = socket(AF_INET, SOCK_STREAM, 0);

	if (m_socket == INVALID_SOCKET)
	{
		WSACleanup();
		return -1;
	}
#else
	m_socket = socket(AF_INET, SOCK_STREAM, 0);

	if (m_socket == INVALID_PORT)
	{
		return -1;
	}
#endif
	// Set up server details
	sockaddr_in serverAddress{};
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_port = htons(m_port);
	if (inet_pton(AF_INET, m_address.c_str(), &(serverAddress.sin_addr)) <= 0)
	{
		return -1;
	}

	// Connect to the server
	if (connect(m_socket, reinterpret_cast<struct sockaddr*>(&serverAddress), sizeof(serverAddress)) < 0)
	{
		Close();
		return -1;
	}

	return 0;
}

int TcpClient::Send(const char* buffer, const size_t size)
{
	int sizeSent = send(m_socket, buffer, static_cast<int>(size), 0);

	if (sizeSent < 0) { return -1; }

	return sizeSent;
}

int TcpClient::Receive(void* buffer, const size_t maxSize)
{
	int sizeRead = recv(m_socket, (char*)buffer, static_cast<int>(maxSize), 0);

	if (sizeRead < 0) { return -1; }

	return sizeRead;
}

void TcpClient::Close()
{
#ifdef WIN32
	closesocket(m_socket);
	WSACleanup();
#else
	close(m_socket);
#endif
	m_socket = INVALID_SOCKET;
}

int TcpClient::ValidateIP(const std::string& ip)
{
	// Regex expression for validating IPv4
	std::regex ipv4("(([0-9]|[1-9][0-9]|1[0-9][0-9]|2[0-4][0-9]|25[0-5])\\.){3}([0-9]|[1-9][0-9]|1[0-9][0-9]|2[0-4][0-9]|25[0-5])");

	// Regex expression for validating IPv6
	std::regex ipv6("((([0-9a-fA-F]){1,4})\\:){7}([0-9a-fA-F]){1,4}");

	// Checking if it is a valid IPv4 addresses
	if (std::regex_match(ip, ipv4))
	{
		return 1;
	}
	// Checking if it is a valid IPv6 addresses
	else if (std::regex_match(ip, ipv6))
	{
		return 2;
	}

	// Return Invalid
	return -1;
}

bool TcpClient::ValidatePort(const uint16_t port)
{
	return (port > 0 && port < 65535);
}