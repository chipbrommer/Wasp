
/////////////////////////////////////////////////////////////////////////////////
/// @file		udp_client.cpp
/// @brief		Implementation of the UDP client.
/// @author		Chip Brommer
/////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////
//
//  Includes:
//          name                        reason included
//          --------------------        ---------------------------------------
#include	"udp_client.h"				// UDP Client Class
//
/////////////////////////////////////////////////////////////////////////////////

UdpClient::UdpClient()
{
	m_name = "UDP Client";
	m_lastError = ErrorCode::NONE;
	m_destinationAddr = {};
	m_clientAddr = {};
	m_broadcastAddr = {};
	m_lastReceiveInfo = new Endpoint();
	m_timeout.tv_sec = UDP_DEFAULT_SOCKET_TIMEOUT;
#if WIN32
	m_timeout.tv_usec = UDP_DEFAULT_SOCKET_TIMEOUT * 1000;
#else
	m_timeout.tv_usec = static_cast<__suseconds_t>(UDP_DEFAULT_SOCKET_TIMEOUT) * 1000;
#endif
	m_timeToLive = 2;

#ifdef WIN32
	if (WSAStartup(MAKEWORD(2, 2), &m_wsaData) != 0)
	{
		m_lastError = ErrorCode::WINSOCK_FAILURE;
	}
#endif
	m_socket = INVALID_SOCKET;
	m_broadcastSocket = INVALID_SOCKET;
}

UdpClient::UdpClient(const std::string& clientsAddress, const int16_t clientsPort)
{
	if (ValidateIP(clientsAddress) == -1)
	{
		m_lastError = ErrorCode::BAD_ADDRESS;
	}

	if (ValidatePort(clientsPort) == false)
	{
		m_lastError = ErrorCode::BAD_PORT;
	}

	// Setup m_clientAddr
	memset(reinterpret_cast<char*>(&m_clientAddr), 0, sizeof(m_clientAddr));
	m_clientAddr.sin_family = AF_INET;
	m_clientAddr.sin_port = htons(clientsPort);
	if (inet_pton(AF_INET, clientsAddress.c_str(), &(m_clientAddr.sin_addr)) <= 0)
	{
		m_lastError = ErrorCode::ADDRESS_NOT_SUPPORTED;
	}

	m_name = "TCP Client";
	m_lastError = ErrorCode::NONE;
	m_destinationAddr = {};
	m_broadcastAddr = {};
	m_timeout.tv_sec = UDP_DEFAULT_SOCKET_TIMEOUT;
#if WIN32
	m_timeout.tv_usec = UDP_DEFAULT_SOCKET_TIMEOUT * 1000;
#else
	m_timeout.tv_usec = static_cast<__suseconds_t>(UDP_DEFAULT_SOCKET_TIMEOUT) * 1000;
#endif
	m_timeToLive = 2;

#ifdef WIN32
	if (WSAStartup(MAKEWORD(2, 2), &m_wsaData) != 0)
	{
		m_lastError = ErrorCode::WINSOCK_FAILURE;
	}
#endif
	m_socket = INVALID_SOCKET;
	m_broadcastSocket = INVALID_SOCKET;
}

UdpClient::~UdpClient()
{
	CloseUnicast();
	CloseBroadcast();
	CloseMulticast();

#ifdef WIN32
	WSACleanup();
#endif
}

bool UdpClient::IsGood() const
{
	return m_socket != INVALID_SOCKET;
}

int8_t UdpClient::ConfigureThisClient(const std::string& address, const int16_t port)
{
	if (ValidateIP(address) == -1)
	{
		m_lastError = ErrorCode::BAD_ADDRESS;
		return -1;
	}

	if (ValidatePort(port) == false)
	{
		m_lastError = ErrorCode::BAD_PORT;
		return -1;
	}

	// Setup m_clientAddr
	memset(reinterpret_cast<char*>(&m_clientAddr), 0, sizeof(m_clientAddr));
	m_clientAddr.sin_family = AF_INET;
	m_clientAddr.sin_port = htons(port);
	if (inet_pton(AF_INET, address.c_str(), &(m_clientAddr.sin_addr)) <= 0)
	{
		m_lastError = ErrorCode::CONFIGURATION_FAILED;
		return -1;
	}

	return 0;
}

int8_t  UdpClient::SetUnicastDestination(const std::string& address, const int16_t port)
{
	if (ValidateIP(address) == -1)
	{
		m_lastError = ErrorCode::BAD_ADDRESS;
		return -1;
	}

	if (ValidatePort(port) == false)
	{
		m_lastError = ErrorCode::BAD_PORT;
		return -1;
	}

	// Setup m_destinationAddr
	memset(reinterpret_cast<char*>(&m_destinationAddr), 0, sizeof(m_destinationAddr));
	m_destinationAddr.sin_family = AF_INET;
	m_destinationAddr.sin_port = htons(port);
	if (inet_pton(AF_INET, address.c_str(), &(m_destinationAddr.sin_addr)) <= 0)
	{
		m_lastError = ErrorCode::SET_DESTINATION_FAILED;
		return -1;
	}

	return 0;
}

int8_t UdpClient::EnableBroadcastSender(const int16_t port)
{
	if (m_broadcastSocket != INVALID_SOCKET)
	{
		m_lastError = ErrorCode::BROADCAST_ALREADY_ENABLED;
		return -1;
	}

	if (!ValidatePort(port))
	{
		m_lastError = ErrorCode::BAD_PORT;
		return -1;
	}

	// Open the broadcast socket
	m_broadcastSocket = socket(AF_INET, SOCK_DGRAM, 0);

	if (m_broadcastSocket == -1)
	{
		m_lastError = ErrorCode::BROADCAST_SOCKET_OPEN_FAILURE;
		return -1;
	}

	// Setup broadcast addr
	memset(reinterpret_cast<char*>(&m_broadcastAddr), 0, sizeof(m_broadcastAddr));
	m_broadcastAddr.sin_family = AF_INET;
	m_broadcastAddr.sin_port = htons(port);
	m_broadcastAddr.sin_addr.s_addr = INADDR_BROADCAST;

	// set broadcast option
	int8_t broadcast = 1;
	if (setsockopt(m_broadcastSocket, SOL_SOCKET, SO_BROADCAST, (char*)&broadcast, sizeof(broadcast)) < 0)
	{
		m_lastError = ErrorCode::ENABLE_BROADCAST_FAILED;
		return -1;
	}

	// success
	return 0;
}

int8_t UdpClient::AddBroadcastListener(const int16_t port)
{
	SOCKET sock = socket(AF_INET, SOCK_DGRAM, 0);

	if (sock == INVALID_SOCKET)
	{
		m_lastError = ErrorCode::BROADCAST_SOCKET_OPEN_FAILURE;
		return -1;
	}

	// Set the receive timeout
	if (setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, reinterpret_cast<const char*>(&m_timeout), sizeof(m_timeout)) == SOCKET_ERROR)
	{
		m_lastError = ErrorCode::FAILED_TO_SET_TIMEOUT;
		return -1;
	}

	sockaddr_in addr{};
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.s_addr = INADDR_ANY;

	if (bind(sock, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) == SOCKET_ERROR)
	{
		m_lastError = ErrorCode::BIND_FAILED;
		return -1;
	}

	Endpoint ep{ "", port };

	m_broadcastListeners.push_back({ sock,addr,ep });

	return 0;
}

int8_t UdpClient::DisableBroadcast()
{
	if (m_broadcastSocket == INVALID_SOCKET)
	{
		m_lastError = ErrorCode::BROADCAST_NOT_ENABLED;
		return -1;
	}

	// reset the sockaddr and close the broadcast socket. 
	memset(reinterpret_cast<char*>(&m_broadcastAddr), 0, sizeof(m_broadcastAddr));
	closesocket(m_broadcastSocket);
	m_broadcastSocket = INVALID_SOCKET;

	return 0;
}

int8_t UdpClient::EnableMulticast(const std::string& groupIP, const int16_t groupPort)
{
	return AddMulticastGroup(groupIP, groupPort);
}

int8_t UdpClient::DisableMulticast()
{
	if (m_multicastSockets.size() < 1)
	{
		m_lastError = ErrorCode::MULTICAST_NOT_ENABLED;
		return -1;
	}

	// success
	CloseMulticast();
	return 0;
}

int8_t UdpClient::AddMulticastGroup(const std::string& groupIP, const int16_t groupPort)
{
	if (ValidateIP(groupIP) == -1)
	{
		m_lastError = ErrorCode::BAD_ADDRESS;
		return -1;
	}

	if (ValidatePort(groupPort) == false)
	{
		m_lastError = ErrorCode::BAD_PORT;
		return -1;
	}

	Endpoint ep = { groupIP, groupPort };

	// Create a UDP socket
	SOCKET sock = socket(AF_INET, SOCK_DGRAM, 0);

	if (sock == INVALID_SOCKET)
	{
		m_lastError = ErrorCode::BAD_MULTICAST_ADDRESS;
		return 1;
	}

	// Enable SO_REUSEADDR to allow multiple sockets to bind to the same address
	int8_t reuseAddr = 1;
	if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (const char*)&reuseAddr, sizeof(reuseAddr)) == SOCKET_ERROR)
	{
		closesocket(sock);
		m_lastError = ErrorCode::ENABLE_REUSEADDR_FAILED;
		return -1;
	}

	// Set up the multicast group to send and receive data from
	sockaddr_in multicastAddr{};
	multicastAddr.sin_family = AF_INET;
	multicastAddr.sin_port = htons(groupPort);
	if (inet_pton(AF_INET, groupIP.c_str(), &(multicastAddr.sin_addr)) <= 0)
	{
		m_lastError = ErrorCode::BAD_MULTICAST_ADDRESS;
		return -1;
	}

	// Bind the socket to a local IP address
	sockaddr_in localAddr{};
	localAddr.sin_family = AF_INET;
	localAddr.sin_port = htons(groupPort);
	localAddr.sin_addr.s_addr = INADDR_ANY;

	// Bind the socket to the multicast address
	if (bind(sock, (sockaddr*)&localAddr, sizeof(localAddr)) < 0)
	{
		m_lastError = ErrorCode::MULTICAST_BIND_FAILED;
		return 1;
	}

	// Set the TTL (time to live) for any outpoing multicast packets to 5 hops
	if (setsockopt(sock, IPPROTO_IP, IP_MULTICAST_TTL, (const char*)&m_timeToLive, sizeof(m_timeToLive)) == SOCKET_ERROR)
	{
		m_lastError = ErrorCode::MULTICAST_SET_TTL_FAILED;
		return -1;
	}

	// Set the outgoing interface for multicast packets
	in_addr interfaceAddr{};
	interfaceAddr.s_addr = INADDR_ANY;
	if (setsockopt(sock, IPPROTO_IP, IP_MULTICAST_IF, (char*)&interfaceAddr, sizeof(interfaceAddr)) < 0)
	{
		m_lastError = ErrorCode::MULTICAST_INTERFACE_ERROR;
		return -1;
	}

	// Join the multicast group
	ip_mreq multicastRequest{};
	if (inet_pton(AF_INET, groupIP.c_str(), &(multicastRequest.imr_multiaddr)) <= 0)
	{
		m_lastError = ErrorCode::BAD_MULTICAST_ADDRESS;
		return -1;
	}

	multicastRequest.imr_interface.s_addr = INADDR_ANY;
	if (setsockopt(sock, IPPROTO_IP, IP_ADD_MEMBERSHIP, (const char*)&multicastRequest, sizeof(multicastRequest)) == SOCKET_ERROR)
	{
		m_lastError = ErrorCode::ADD_MULTICAST_GROUP_FAILED;
		closesocket(sock);
		return -1;
	}

	// Set the socket to non-blocking mode
#ifdef WIN32
	u_long mode = 1;
	if (ioctlsocket(sock, FIONBIO, &mode) != 0)
	{
		closesocket(sock);
		return -1;
	}
#else
	int flags = fcntl(sock, F_GETFL, 0);
	if (flags == -1)
	{
		m_lastError = ErrorCode::FAILED_TO_GET_SOCKET_FLAGS;
		return -1;
	}

	if (fcntl(sock, F_SETFL, flags | O_NONBLOCK) == -1)
	{
		m_lastError = ErrorCode::FAILED_TO_SET_NONBLOCK;
		return -1;
	}
#endif

	m_multicastSockets.push_back({ sock, multicastAddr, ep });

	return 0;
}

int8_t UdpClient::OpenUnicast()
{
	if (m_socket != -1)
	{
		m_lastError = ErrorCode::CLIENT_ALREADY_CONNECTED;
		return -1;
	}

	m_socket = socket(AF_INET, SOCK_DGRAM, 0);

	if (m_socket == INVALID_SOCKET)
	{
		m_lastError = ErrorCode::SOCKET_OPEN_FAILURE;
		return -1;
	}

	// Set as nonblocking socket
#ifdef WIN32
	u_long nonBlockingMode = 1;
	if (ioctlsocket(m_socket, FIONBIO, &nonBlockingMode) != 0)
	{
		m_lastError = ErrorCode::FAILED_TO_SET_NONBLOCK;
		return -1;
	}
#else
	int flags = fcntl(m_socket, F_GETFL, 0);
	if (flags == -1)
	{
		m_lastError = ErrorCode::FAILED_TO_GET_SOCKET_FLAGS;
		return -1;
	}

	if (fcntl(m_socket, F_SETFL, flags | O_NONBLOCK) == -1)
	{
		m_lastError = ErrorCode::FAILED_TO_SET_NONBLOCK;
		return -1;
	}
#endif
	// Set reuseable address. 
	int8_t opt = 1;
	if (setsockopt(m_socket, SOL_SOCKET, SO_REUSEADDR, (const char*)&opt, sizeof(opt)) < 0)
	{
		m_lastError = ErrorCode::ENABLE_REUSEADDR_FAILED;
		return -1;
	}

	if (bind(m_socket, (sockaddr*)&m_clientAddr, sizeof(m_clientAddr)) < 0)
	{
		m_lastError = ErrorCode::BIND_FAILED;
		return -1;
	}

	return 0;
}

int8_t UdpClient::Send(const char* buffer, const uint32_t size, const SendType type)
{
	switch (type)
	{
	case SendType::UNICAST:		return SendUnicast(buffer, size);	break;
	case SendType::BROADCAST:	return SendBroadcast(buffer, size); break;
	case SendType::MULTICAST:	return SendMulticast(buffer, size); break;
	default: return -1;
	}

	// default return
	return -1;
}

int8_t UdpClient::SendUnicast(const char* buffer, const uint32_t size)
{
	// verify socket and then send datagram
	if (m_socket != INVALID_SOCKET)
	{
		int32_t numSent = sendto(m_socket, buffer, size, 0, (sockaddr*)&m_destinationAddr, sizeof(m_destinationAddr));

		if (numSent == -1)
		{
			m_lastError = ErrorCode::SEND_FAILED;
			return -1;
		}

		// return success
		return numSent;
	}

	// default return
	return -1;
}

int8_t UdpClient::SendUnicast(const char* buffer, const uint32_t size, const std::string& ipAddress, const int16_t port)
{
	// verify socket and then send datagram
	if (m_socket != INVALID_SOCKET)
	{
		if (ValidateIP(ipAddress) == -1)
		{
			m_lastError = ErrorCode::BAD_ADDRESS;
			return -1;
		}

		if (ValidatePort(port) == false)
		{
			m_lastError = ErrorCode::BAD_PORT;
			return -1;
		}

		// Setup m_destinationAddr
		sockaddr_in sentTo{};
		memset(reinterpret_cast<char*>(&sentTo), 0, sizeof(sentTo));
		sentTo.sin_family = AF_INET;
		sentTo.sin_port = htons(port);
		if (inet_pton(AF_INET, ipAddress.c_str(), &(sentTo.sin_addr)) <= 0)
		{
			m_lastError = ErrorCode::SET_DESTINATION_FAILED;
			return -1;
		}

		int32_t numSent = 0;
		numSent = sendto(m_socket, buffer, size, 0, (sockaddr*)&sentTo, sizeof(sentTo));

		if (numSent == -1)
		{
			m_lastError = ErrorCode::SEND_FAILED;
			return -1;
		}

		// return success
		return numSent;
	}

	// default return
	return -1;
}

int8_t UdpClient::SendBroadcast(const char* buffer, const uint32_t size)
{
	// verify socket and then send datagram
	if (m_broadcastSocket != INVALID_SOCKET)
	{
		int32_t numSent = sendto(m_broadcastSocket, buffer, size, 0, (sockaddr*)&m_broadcastAddr, sizeof(sockaddr_in));

		if (numSent == -1)
		{
			m_lastError = ErrorCode::SEND_BROADCAST_FAILED;
			return -1;
		}

		// return success
		return numSent;
	}

	// default return
	return -1;
}

int8_t UdpClient::SendMulticast(const char* buffer, const uint32_t size, const std::string& groupIP)
{
	// verify socket and then send datagram
	if (m_multicastSockets.size() > 0)
	{
		int32_t numSent = 0;
		for (const auto& i : m_multicastSockets)
		{
			// Grab the socket and addr info from the vector for use.
			SOCKET sock = std::get<0>(i);
			sockaddr_in addr = std::get<1>(i);
			Endpoint ep = std::get<2>(i);

			// If groupIP is not empty, check the IP we are currently sending to and only send to the desired group.
			if (!groupIP.empty() && groupIP != ep.ipAddress)
			{
				continue;
			}

			numSent = sendto(sock, buffer, size, 0, (sockaddr*)&addr, sizeof(sockaddr_in));

			if (numSent < 0)
			{
				m_lastError = ErrorCode::SEND_MULTICAST_FAILED;
				return false;
			}
		}

		return numSent;
	}

	return -1;
}

int8_t UdpClient::ReceiveUnicast(void* buffer, const uint32_t maxSize)
{
	// Store the data source info
	sockaddr_in sourceAddress{};
	int addressLength = sizeof(sourceAddress);

	// Receive datagram over UDP
#if defined WIN32
	int32_t sizeRead = recvfrom(m_socket, reinterpret_cast<char*>(buffer), maxSize - 1, 0, (sockaddr*)&sourceAddress, &addressLength);
#else
	int32_t sizeRead = recvfrom(m_socket, buffer, static_cast<size_t>(maxSize) - 1, 0, (sockaddr*)&sourceAddress, reinterpret_cast<socklen_t*>(&addressLength));
#endif

	// Check for error
	if (sizeRead == -1)
	{
#ifdef WIN32
		int errorCode = WSAGetLastError();
		if (errorCode != WSAEWOULDBLOCK)
		{
			m_lastError = ErrorCode::READ_FAILED;
			return -1;
		}
#else
		if (errno != EWOULDBLOCK)
		{
			m_lastError = ErrorCode::READ_FAILED;
			return -1;
		}
#endif
		return 0;
	}

	// if data was received, store the port and ip for history. 
	if (sizeRead > 0)
	{
		m_lastReceiveInfo->port = ntohs(sourceAddress.sin_port);

		char addr[INET_ADDRSTRLEN];
		if (inet_ntop(AF_INET, &(sourceAddress.sin_addr), addr, INET_ADDRSTRLEN) != NULL)
		{
			m_lastReceiveInfo->ipAddress = addr;
		}
	}

	// return size read
	return sizeRead;
}

int8_t UdpClient::ReceiveUnicast(void* buffer, const uint32_t maxSize, std::string& recvFromAddr, int16_t& recvFromPort)
{
	int8_t rtn = ReceiveUnicast(buffer, maxSize);

	if (rtn > 0)
	{
		recvFromAddr = GetIpOfLastReceive();
		recvFromPort = GetPortOfLastReceive();
	}

	return rtn;;
}

int8_t UdpClient::ReceiveBroadcast(void* buffer, const uint32_t maxSize)
{
	if (m_broadcastListeners.size() > 0)
	{
		for (const auto& i : m_broadcastListeners)
		{
			// Grab the socket and addr info from the vector for use.
			SOCKET sock = std::get<0>(i);
			sockaddr_in addr = std::get<1>(i);

			if (sock != INVALID_SOCKET)
			{
				// Verify incoming data is available.
				fd_set readSet{};
				FD_ZERO(&readSet);
				FD_SET(sock, &readSet);
				sockaddr_in recvFrom{};
				int recvFromSize = sizeof(recvFrom);

				int selectResult = select((int)sock + 1, &readSet, nullptr, nullptr, &m_timeout);

				// Catch error
				if (selectResult == SOCKET_ERROR)
				{
					m_lastError = ErrorCode::SELECT_READ_ERROR;
					return 1;
				}

				// If data is available on this socket, attempt to read it 
				if (selectResult > 0)
				{
#if defined WIN32
					int32_t receivedBytes = recvfrom(sock, (char*)buffer, maxSize - 1, 0, reinterpret_cast<sockaddr*>(&recvFrom), &recvFromSize);
#else
					int32_t receivedBytes = recvfrom(m_socket, buffer, static_cast<size_t>(maxSize) - 1, 0, (sockaddr*)&recvFrom, reinterpret_cast<socklen_t*>(&recvFromSize));
#endif

					if (receivedBytes == SOCKET_ERROR)
					{
#ifdef WIN32
						int errorCode = WSAGetLastError();
						if (errorCode != WSAEWOULDBLOCK)
						{
							m_lastError = ErrorCode::RECEIVE_BROADCAST_FAILED;
							return -1;
						}
#else
						if (errno != EWOULDBLOCK)
						{
							m_lastError = ErrorCode::RECEIVE_BROADCAST_FAILED;
							return -1;
						}
#endif
						return 0;
					}

					m_lastRecvBroadcastPort = ntohs(addr.sin_port);
					return receivedBytes;
				}

				// If here, selectResult == 0, so we check next socket. 
			}
		}

		// If here, for loop completed, and all have no data. 
		return 0;
	}
	return -1;
}

int8_t UdpClient::ReceiveBroadcast(void* buffer, const uint32_t maxSize, int16_t& port)
{
	int rtn = ReceiveBroadcast(buffer, maxSize);

	if (rtn >= 0)
	{
		port = m_lastRecvBroadcastPort;
	}
	return rtn;
}

int8_t UdpClient::ReceiveBroadcastFromListenerPort(void* buffer, const uint32_t maxSize, const int16_t port)
{
	if (m_broadcastListeners.size() > 0)
	{
		for (const auto& i : m_broadcastListeners)
		{
			// Grab the socket and addr info from the vector for use.
			SOCKET sock = std::get<0>(i);
			sockaddr_in addr = std::get<1>(i);
			Endpoint ep = std::get<2>(i);

			// If the ports arent equal, continue to next iteration.
			if (port != ep.port)
			{
				continue;
			}

			if (sock != INVALID_SOCKET)
			{
				// Verify incoming data is available.
				fd_set readSet{};
				FD_ZERO(&readSet);
				FD_SET(sock, &readSet);
				sockaddr_in recvFrom{};
				int recvFromSize = sizeof(recvFrom);

				int selectResult = select((int)sock + 1, &readSet, nullptr, nullptr, &m_timeout);

				// Catch error
				if (selectResult == SOCKET_ERROR)
				{
					m_lastError = ErrorCode::SELECT_READ_ERROR;
					return 1;
				}

				// If data is available on this socket, attempt to read it 
				if (selectResult > 0)
				{
#if defined WIN32
					int32_t receivedBytes = recvfrom(sock, (char*)buffer, maxSize - 1, 0, reinterpret_cast<sockaddr*>(&recvFrom), &recvFromSize);
#else
					int32_t receivedBytes = recvfrom(m_socket, buffer, static_cast<size_t>(maxSize) - 1, 0, (sockaddr*)&recvFrom, reinterpret_cast<socklen_t*>(&recvFromSize));
#endif

					if (receivedBytes == SOCKET_ERROR)
					{
#ifdef WIN32
						int errorCode = WSAGetLastError();
						if (errorCode != WSAEWOULDBLOCK)
						{
							m_lastError = ErrorCode::RECEIVE_BROADCAST_FAILED;
							return -1;
						}
#else
						if (errno != EWOULDBLOCK)
						{
							m_lastError = ErrorCode::RECEIVE_BROADCAST_FAILED;
							return -1;
						}
#endif
						return 0;
					}

					m_lastRecvBroadcastPort = ntohs(addr.sin_port);
					return receivedBytes;
				}

				// If here, selectResult == 0, so we check next socket. 
			}
		}

		// If here, for loop completed, and all have no data. 
		return 0;
	}
	return -1;
}

int8_t UdpClient::ReceiveMulticast(void* buffer, const uint32_t maxSize, std::string& multicastGroup)
{
	if (m_multicastSockets.size() > 0)
	{
		for (const auto& i : m_multicastSockets)
		{
			// Grab the socket and addr info from the vector for use.
			SOCKET sock = std::get<0>(i);
			sockaddr_in addr = std::get<1>(i);

			if (sock != INVALID_SOCKET)
			{
				// Verify incoming data is available.
				fd_set readSet{};
				FD_ZERO(&readSet);
				FD_SET(sock, &readSet);
				sockaddr_in recvFrom{};
				int recvFromSize = sizeof(recvFrom);

				int selectResult = select(0, &readSet, nullptr, nullptr, &m_timeout);

				// Catch error
				if (selectResult == SOCKET_ERROR)
				{
					m_lastError = ErrorCode::SELECT_READ_ERROR;
					return 1;
				}

				// If data is available on this socket, attempt to read it 
				if (selectResult > 0)
				{
#if defined WIN32
					int32_t receivedBytes = recvfrom(sock, (char*)buffer, maxSize - 1, 0, reinterpret_cast<sockaddr*>(&recvFrom), &recvFromSize);
#else
					int32_t receivedBytes = recvfrom(m_socket, buffer, static_cast<size_t>(maxSize) - 1, 0, (sockaddr*)&recvFrom, reinterpret_cast<socklen_t*>(&recvFromSize));
#endif

					if (receivedBytes == SOCKET_ERROR)
					{
#ifdef WIN32
						int errorCode = WSAGetLastError();
						if (errorCode != WSAEWOULDBLOCK)
						{
							m_lastError = ErrorCode::RECEIVE_BROADCAST_FAILED;
							return -1;
						}
#else
						if (errno != EWOULDBLOCK)
						{
							m_lastError = ErrorCode::RECEIVE_BROADCAST_FAILED;
							return -1;
						}
#endif
						return 0;
					}

					char address[INET_ADDRSTRLEN];
					if (inet_ntop(AF_INET, &(addr.sin_addr), address, INET_ADDRSTRLEN) != NULL)
					{
						multicastGroup = address;
					}

					return receivedBytes;
				}

				// If here, selectResult == 0, so we check next socket. 
			}
		}

		// If here, for loop completed, and all have no data. 
		return 0;
	}
	return -1;
}

void UdpClient::CloseUnicast()
{
	closesocket(m_socket);
	m_socket = INVALID_SOCKET;
}

void UdpClient::CloseBroadcast()
{
	closesocket(m_broadcastSocket);
	m_broadcastSocket = INVALID_SOCKET;

	for (const auto& i : m_broadcastListeners)
	{
		closesocket(std::get<0>(i));
	}

	m_broadcastListeners.clear();
}

void UdpClient::CloseMulticast()
{
	for (const auto& i : m_multicastSockets)
	{
		closesocket(std::get<0>(i));
	}

	m_multicastSockets.clear();
}

int8_t UdpClient::SetTimeToLive(const int8_t ttl)
{
	if (ttl > 0 && ttl < 255)
	{
		m_timeToLive = ttl;

		if (m_multicastSockets.size() > 0)
		{
			for (const auto& i : m_multicastSockets)
			{
				SOCKET sock = std::get<0>(i);

				if (setsockopt(sock, IPPROTO_IP, IP_MULTICAST_TTL, (const char*)&m_timeToLive, sizeof(m_timeToLive)) == SOCKET_ERROR)
				{
					m_lastError = ErrorCode::MULTICAST_SET_TTL_FAILED;
					return -1;
				}
			}
		}

		return 0;
	}

	return -1;
}

int8_t UdpClient::SetTimeout(const int32_t timeoutMSecs)
{
	m_timeout.tv_sec = timeoutMSecs / 1000;
#if WIN32
	m_timeout.tv_usec = timeoutMSecs * 1000;
#else
	m_timeout.tv_usec = static_cast<__suseconds_t>(timeoutMSecs) * 1000;
#endif
	return 0;
}

std::string UdpClient::GetIpOfLastReceive()
{
	return m_lastReceiveInfo->ipAddress;
}

int16_t UdpClient::GetPortOfLastReceive()
{
	return m_lastReceiveInfo->port;
}

std::string UdpClient::GetLastError()
{
	return UdpClientErrorMap[m_lastError];
}

int UdpClient::ValidateIP(const std::string& ip)
{
	sockaddr_in sa4 = {};
	sockaddr_in6 sa6 = {};

	// Check if it's a valid IPv4 address
	if (inet_pton(AF_INET, ip.c_str(), &(sa4.sin_addr)) == 1)
	{
		return 1;
	}

	// Check if it's a valid IPv6 address
	if (inet_pton(AF_INET6, ip.c_str(), &(sa6.sin6_addr)) == 1)
	{
		return 2;
	}

	return -1;  // Invalid IP address
}

bool UdpClient::ValidatePort(const int16_t port)
{
	return (port >= 0 && port <= 65535);
}