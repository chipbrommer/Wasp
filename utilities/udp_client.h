#pragma once
/////////////////////////////////////////////////////////////////////////////////
/// @file		udp_client.h
/// @brief		A cross platform class to handle UDP communication.
/// @author		Chip Brommer
/////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////
//
//  Includes:
//          name                        reason included
//          --------------------        ---------------------------------------
#ifdef WIN32
#include <stdint.h>						// Standard integer types
#include <WinSock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
typedef int SOCKET;
typedef struct sockaddr_in SOCKADDR_IN;
typedef struct sockaddr SOCKADDR;
const int INVALID_SOCKET = -1;
const int SOCKET_ERROR = -1;
const int SD_BOTH = SHUT_RDWR;
#define closesocket(s) close(s)
#endif
#include <map>							// Error enum to strings.
#include <string>						// Strings
#include <regex>						// Regular expression for ip validation
//
/////////////////////////////////////////////////////////////////////////////////

class UdpClient
{
public:
	/// @brief enum for error codes
	enum class ErrorCode : int
	{
		NONE,
		BAD_ADDRESS,
		ADDRESS_NOT_SET,
		BAD_PORT,
		PORT_NOT_SET,
		CLIENT_ALREADY_CONNECTED,
		FAILED_TO_CONNECT,
		WINSOCK_FAILURE,
		SOCKET_OPEN_FAILURE,
		ADDRESS_NOT_SUPPORTED,
		CONNECTION_FAILED,
		SEND_FAILED,
		READ_FAILED,
		ENABLE_MULTICAST_FAILED,
		DISABLE_MULTICAST_FAILED,
		ENABLE_BROADCAST_FAILED,
		DISABLE_BROADCAST_FAILED,
		SEND_MULTICAST_FAILED,
		SEND_BROADCAST_FAILED,
		CONFIGURATION_FAILED,
		SET_DESTINATION_FAILED,
		BIND_FAILED,
		BROADCAST_ALREADY_ENABLED,
		BROADCAST_SOCKET_OPEN_FAILURE,
		BROADCAST_NOT_ENABLED,
		MULTICAST_SOCKET_FAILED,
		BAD_MULTICAST_ADDRESS,
		FAILED_TO_SET_NONBLOCK,
		FAILED_TO_GET_SOCKET_FLAGS,
		ENABLE_REUSEADDR_FAILED,
		FAILED_TO_SET_TIMEOUT,
		SELECT_READ_ERROR,
		RECEIVE_BROADCAST_FAILED,
		MULTICAST_NOT_ENABLED,
		ADD_MULTICAST_GROUP_FAILED,
		MULTICAST_INTERFACE_ERROR,
		MULTICAST_BIND_FAILED,
		MULTICAST_SET_TTL_FAILED,
	};

	/// @brief Send Type for the Send Function.
	enum class SendType : uint8_t
	{
		UNICAST,
		BROADCAST,
		MULTICAST,
	};

	/// @brief Default Constructor
	UdpClient();

	/// @brief Constructor to receive an address and port
	UdpClient(const std::string& clientsAddress, const int16_t clientsPort);

	/// @brief Default Deconstructor
	~UdpClient();

	/// @brief Check if the main socket is good for communications
	/// @return true if good, else false (Invalid Socket). 
	bool IsGood() const;

	/// @brief Configure the address and port of this client
	/// @param address -[in]- Address of this client
	/// @param port -[in]- Address of this client
	/// @return 0 if successful, -1 if fails. Call GetLastError() to find out more.
	int8_t ConfigureThisClient(const std::string& address, const int16_t port);

	/// @brief Set this unicast destination
	/// @param address -[in]- Address to sent to
	/// @param port -[in]- Port to sent to
	/// @return 0 if successful, -1 if fails. Call GetLastError() to find out more.
	int8_t SetUnicastDestination(const std::string& address, const int16_t port);

	/// @brief A function to enable broadcasting
	/// @param port -[in]- Port to broadcast on
	//// @return 0 if successful, -1 if fails. Call GetLastError() to find out more.
	int8_t EnableBroadcastSender(const int16_t port);

	/// @brief A function to add a port to listen for broadcast messages
	/// @param port -[in]- Port to listen for broadcast on
	//// @return 0 if successful, -1 if fails. Call GetLastError() to find out more.
	int8_t AddBroadcastListener(const int16_t port);

	/// @brief Disables broadcast and cleans up
	/// @return 0 if successful, -1 if fails. Call GetLastError() to find out more.
	int8_t DisableBroadcast();

	/// @brief Enables multicast sockets and adds the first group. To add more groups, use AddMulticastGroup
	/// @param groupIP -[in]- Address of multicast group.
	/// @param groupPort -[in]- Port of multicast group.
	/// @return 0 if successful, -1 if fails. Call GetLastError() to find out more.
	int8_t EnableMulticast(const std::string& groupIP, const int16_t groupPort);

	/// @brief Disables multicast and cleans up
	/// @return 0 if successful, -1 if fails. Call GetLastError() to find out more.
	int8_t DisableMulticast();

	/// @brief Add an enpoint to the list of multicast recepients.
	/// @param groupIP -[in]- Address of multicast group.
	/// @param groupPort -[in]- Port of multicast group.
	/// @return 0 if successful, -1 if fails. Call GetLastError() to find out more.
	int8_t AddMulticastGroup(const std::string& groupIP, const int16_t port);

	/// @brief Opens the UDP unicast socket and binds it to the set address and port
	/// @return 0 if successful, -1 if fails. Call GetLastError() to find out more.
	int8_t OpenUnicast();

	/// @brief Send a message over a specified socket type
	/// @param buffer -[in]- Buffer to be sent
	/// @param size -[in]- Size to be sent
	/// @return 0+ if successful (number bytes sent), -1 if fails. Call GetLastError() to find out more.
	int8_t Send(const char* buffer, const uint32_t size, const SendType type);

	/// @brief Sends a unicast message
	/// @param buffer -[in]- Buffer to be sent
	/// @param size -[in]- Size to be sent
	/// @return 0+ if successful (number bytes sent), -1 if fails. Call GetLastError() to find out more.
	int8_t SendUnicast(const char* buffer, const uint32_t size);

	/// @brief Send a unicast message to specified ip and port
	/// @param buffer -[in]- Buffer to be sent
	/// @param size -[in]- Size to be sent
	/// @return 0+ if successful (number bytes sent), -1 if fails. Call GetLastError() to find out more.
	int8_t SendUnicast(const char* buffer, const uint32_t size, const std::string& ipAddress, const int16_t port);

	/// @brief Send a broadcast message
	/// @param buffer -[in]- Buffer to be sent
	/// @param size -[in]- Size to be sent
	/// @return 0+ if successful (number bytes sent), -1 if fails. Call GetLastError() to find out more.
	int8_t SendBroadcast(const char* buffer, const uint32_t size);

	/// @brief Send a multicast message to all joined groups
	/// @param buffer -[in]- Buffer to be sent
	/// @param size -[in]- Size to be sent
	/// @param groupIP -[in/opt]- IP of group to send to if only sending to one desired group
	/// @return 0+ if successful (number bytes sent), -1 if fails. Call GetLastError() to find out more.
	int8_t SendMulticast(const char* buffer, const uint32_t size, const std::string& groupIP = "");

	/// @brief Receive data from a server
	/// @param buffer -[out]- Buffer to place received data into
	/// @param maxSize -[in]- Maximum number of bytes to be read
	/// @return 0+ if successful (number bytes received), -1 if fails. Call GetLastError() to find out more.
	int8_t ReceiveUnicast(void* buffer, const uint32_t maxSize);

	/// @brief Receive data from a server and get the IP and Port of the sender
	/// @param "buffer"> -[out]- Buffer to place received data into
	/// @param "maxSize"> -[in]- Maximum number of bytes to be read
	/// @param "recvFromAddr"> -[out]- IP Address of the sender
	/// @param "recvFromPort"> -[out]- Port of the sender
	/// @return 0+ if successful (number bytes received), -1 if fails. Call GetLastError() to find out more.
	int8_t ReceiveUnicast(void* buffer, const uint32_t maxSize, std::string& recvFromAddr, int16_t& recvFromPort);

	/// @brief Receive a broadcast message
	/// @param buffer -[out]- Buffer to place received data into
	/// @param maxSize -[in]- Maximum number of bytes to be read
	/// @return 0+ if successful (number bytes received), -1 if fails. Call GetLastError() to find out more.
	int8_t ReceiveBroadcast(void* buffer, const uint32_t maxSize);

	/// @brief Receive a broadcast message
	/// @param buffer -[out]- Buffer to place received data into
	/// @param maxSize -[in]- Maximum number of bytes to be read
	/// @param port -[out]- Port the broadcast was received from
	/// @return 0+ if successful (number bytes received), -1 if fails. Call GetLastError() to find out more.
	int8_t ReceiveBroadcast(void* buffer, const uint32_t maxSize, int16_t& port);

	/// @brief Receive a broadcast message from a specific listener port
	/// @param buffer -[out]- Buffer to place received data into
	/// @param maxSize -[in]- Maximum number of bytes to be read
	/// @param port -[in]- Port of the broadcast to receive from
	/// @return 0+ if successful (number bytes received), -1 if fails. Call GetLastError() to find out more.
	int8_t ReceiveBroadcastFromListenerPort(void* buffer, const uint32_t maxSize, const int16_t port);

	/// @brief Receive a multicast message
	/// @param buffer -[out]- Buffer to place received data into
	/// @param maxSize -[in]- Maximum number of bytes to be read
	/// @param multicastGroup -[out]- IP of the group received from
	/// @return 0+ if successful (number bytes received), -1 if fails. Call GetLastError() to find out more.
	int8_t ReceiveMulticast(void* buffer, const uint32_t maxSize, std::string& multicastGroup);

	/// @brief Closes the unicast client and cleans up
	void CloseUnicast();

	/// @brief Closes the broadcast client and cleans up
	void CloseBroadcast();

	/// @brief Closes the broadcast client and cleans up
	void CloseMulticast();

	/// @brief Sets the number of hops outbound multicast messages live for.
	/// @param ttl -[in]- Number of hops multicast messages live for 0-255
	/// @return 0 if successful set, -1 if fails. Call GetLastError() to find out more..
	int8_t SetTimeToLive(const int8_t ttl);

	/// @brief Sets the timeout value for message reads.
	/// @param timeoutMSecs -[in]- Number of milliseconds for a read timeout.
	/// @return 0 if successful set, -1 if fails. Call GetLastError() to find out more.
	int8_t SetTimeout(const int32_t timeoutMSecs);

	/// @brief Get the ip address of the last received message.
	/// @return If valid, A string containing the IP address; else an empty string. Call GetLastError() to find out more.
	std::string GetIpOfLastReceive();

	/// @brief Get the port number of the last received message.
	/// @return The port number, else -1 on error. Call GetLastError() to find out more.
	int16_t GetPortOfLastReceive();

	/// @brief Get the last error in string format
	/// @return The last error in a formatted string
	std::string GetLastError();

protected:
private:
	constexpr static uint8_t	UDP_DEFAULT_SOCKET_TIMEOUT = 1;

	/// @brief Error enum to string map
	std::map<ErrorCode, std::string> UdpClientErrorMap
	{
		{ErrorCode::NONE,
		std::string("Error Code " + std::to_string((uint8_t)ErrorCode::NONE) + ": No error.")},
		{ErrorCode::BAD_ADDRESS,
		std::string("Error Code " + std::to_string((uint8_t)ErrorCode::BAD_ADDRESS) + ": Bad address.")},
		{ErrorCode::ADDRESS_NOT_SET,
		std::string("Error Code " + std::to_string((uint8_t)ErrorCode::ADDRESS_NOT_SET) + ": Address not set.")},
		{ErrorCode::BAD_PORT,
		std::string("Error Code " + std::to_string((uint8_t)ErrorCode::BAD_PORT) + ": Bad port.")},
		{ErrorCode::PORT_NOT_SET,
		std::string("Error Code " + std::to_string((uint8_t)ErrorCode::PORT_NOT_SET) + ": Port not set.")},
		{ErrorCode::FAILED_TO_CONNECT,
		std::string("Error Code " + std::to_string((uint8_t)ErrorCode::FAILED_TO_CONNECT) + ": Failed to connect.")},
		{ErrorCode::WINSOCK_FAILURE,
		std::string("Error Code " + std::to_string((uint8_t)ErrorCode::WINSOCK_FAILURE) + ": Winsock creation failure.")},
		{ErrorCode::SOCKET_OPEN_FAILURE,
		std::string("Error Code " + std::to_string((uint8_t)ErrorCode::SOCKET_OPEN_FAILURE) + ": Socket open failure.")},
		{ErrorCode::ADDRESS_NOT_SUPPORTED,
		std::string("Error Code " + std::to_string((uint8_t)ErrorCode::ADDRESS_NOT_SUPPORTED) + ": Address not supported.")},
		{ErrorCode::CONNECTION_FAILED,
		std::string("Error Code " + std::to_string((uint8_t)ErrorCode::CONNECTION_FAILED) + ": Connection failed.")},
		{ErrorCode::SEND_FAILED,
		std::string("Error Code " + std::to_string((uint8_t)ErrorCode::SEND_FAILED) + ": Send failed.")},
		{ErrorCode::READ_FAILED,
		std::string("Error Code " + std::to_string((uint8_t)ErrorCode::READ_FAILED) + ": Read failed.")},
	};

	/// @brief Represents an endpoint for a connection
	struct Endpoint
	{
		std::string ipAddress = "";
		int16_t	port = 0;
	};

	/// @brief Validates an IP address is IPv4 or IPv6
	/// @param ip -[in]- IP Address to be validated
	/// @return 1 if valid ipv4, 2 if valid ipv6, else -1 on fail
	int ValidateIP(const std::string& ip);

	/// @brief Validates a port number is between 0-65535
	/// @param port -[in]- Port number to be validated
	/// @return true = valid, false = invalid
	bool ValidatePort(const int16_t port);

	// Variables
	std::string			m_name;						// Title for this utility when using CPP_Logger
	ErrorCode			m_lastError;				// Last error for this utility
	sockaddr_in			m_destinationAddr;			// Destination sockaddr
	sockaddr_in			m_clientAddr;				// This clients sockaddr
	sockaddr_in			m_broadcastAddr;			// Broadcast sockaddr
	Endpoint*			m_lastReceiveInfo;			// Last receive endpoint info
	timeval				m_timeout;					// Holds the message receive timeout value in seconds. 
	int8_t				m_timeToLive;				// Holds the ttl (Time To Live) for multicast messages. IE: How many interface hops they live for: 0-255
	int16_t				m_lastRecvBroadcastPort;	// Holds port of last received broadcast port

#ifdef WIN32
	WSADATA				m_wsaData;					// Winsock data
#endif
	SOCKET				m_socket;					// socket FD for this client
	SOCKET				m_broadcastSocket;			// socket FD for broadcasting
	std::vector<std::tuple<SOCKET, sockaddr_in, Endpoint>>   m_broadcastListeners;	// Vector of tuples containing the socket and addr info for listening to broadcasts
	std::vector<std::tuple<SOCKET, sockaddr_in, Endpoint>>   m_multicastSockets;	// Vector of tuples containing the socket and addr info for multicasts
};