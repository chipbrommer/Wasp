#pragma once
/////////////////////////////////////////////////////////////////////////////////
// @file            web_server.h
// @brief           Class to implement a web server
// @author          Chip Brommer
/////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////
// 
// Includes:
//          name                    reason included
//          ------------------      ------------------------
#include <string>                   // strings
//
#include "../external/civetweb/civetweb.h"		// Civet header
#include "../external/inja/inja.hpp"			// inja header
#include "log_client.h"             // Log Client
#include "constants.h"				// Buffer size
#include <mutex>
#include "version.h"				// Generated version file
#include "../web_pages/web_pages.h"	// web pages
//
/////////////////////////////////////////////////////////////////////////////////

class WebServer
{
public:
	enum class Page
	{
		Config,
		Data,
		Dev,
		Index,
		Login,
		Reboot,
		Update,
		Error
	};

	/// @brief Constructor
	/// @param logger - instance of LogClient
	/// @param port - port to serve the server at
	/// @param directory - directory for the server files
	/// @param maxThreads - maximum number of threads for the web server
	/// @param websocketName - name for the websocket connection
	WebServer(LogClient& logger, const int port = 8080, const std::string directory = ".", 
		const int maxThreads = 5, const std::string websocketName = "websocket");

	/// @brief Default deconstructor
	~WebServer();

	/// @brief Sets the port for 
	/// @param port - [in] - Port to server the server on
	void SetServerPort(int port);

	/// @brief Sets the target directory for web requests
	/// @param path - [in] - path for the webserver files
	void SetServerDirectory(std::string directory);

	/// @brief Configure the webserver with a port and a path
	/// @param port - [in] - Port to server the server on
	/// @param path - [in] - path for the webserver files
	void Configure(int port, std::string directory);

	/// @brief Starts the web server process. NOTE: This blocks the calling thread
	void Start();

	/// @brief Sends a JSON structure over WebSocket
	/// @param json - [in] - JSON structure to send
	/// @return 0 on error, else the total number of messages sent to websockets
	int SendJsonOverWebSocket(const nlohmann::json& json);

	/// @brief Sends a string over WebSocket
	/// @param msg - [in] - string to send
	/// @return 0 on error, else the total number of messages sent to websockets
	int SendMessageOverWebSocket(const std::string& msg);

	/// @brief Stops the web server process.
	void Stop();

	/// @brief handles page requests
	/// @param c - the connection for the page request
	/// @param page - the specific page to be handled
	void HandlePage(mg_connection* c, Page page);

	/// @brief handles page requests
	/// @param c - the connection for the page request
	/// @param page - the custom string page to be handled
	void HandleCustomPage(mg_connection* c, std::string page);

	/// @brief handles config page request
	/// @param c - the connection for the page request
	void HandleConfigPage(mg_connection* c);

	/// @brief handles the layout page request
	/// @param c - the connection for the page request
	/// @param bodyContent - the content to be embedded in the layout page
	void HandleLayoutPage(mg_connection* c, std::string bodyContent);


protected:

private:
	/// @brief Attempts to parse a specified variable from the specified content
	/// @param name - [in] - the variable to parse
	/// @param data - [in] - the data to parse the variable from
	/// @return - the string format of the variable value
	std::string Parse(const std::string name, const char* data);

	int WebSocketConnectHandler(const mg_connection* conn);

	void WebSocketReadyHandler(const mg_connection* conn);

	int WebsocketDataHandler(const mg_connection* conn, int bits, char* data, size_t data_len);

	void WebSocketCloseHandler(const mg_connection* conn);

	std::atomic_bool	m_run					= false;
    std::string			m_name					= "";
    LogClient&			m_logger;
    int					m_port					= 0;
    std::string			m_directory				= "";
	std::string			m_websocketName			= "";
	mg_context*			m_context				= {};
	std::string			m_listeningAddress		= "";
	char				m_buffer[BUFFER_SIZE]	= {};
	int					m_maxThreads			= 0;
	inja::Environment	m_environment;
	long				m_sendErrorCount		= 0;

	// Items for websocket
	std::mutex								m_connectionLock	= {};
	std::map<mg_connection*, bool>			m_connections		= {};
};