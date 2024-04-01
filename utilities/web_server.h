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
#include "../external/civetweb/CivetServer.h"   // Civet header
#include "log_client.h"             // Log Client
#include "constants.h"				// Buffer size
//
/////////////////////////////////////////////////////////////////////////////////

class WebServer
{
public:

    /// @brief Constructor
    /// @param logger - instance of LogClient
    /// @param port - port to serve the server at
    /// @param directory - directory for the server files
    WebServer(LogClient& logger, const int port = 8080, const std::string directory = ".") :
        m_name("WEB SVR"), m_server(nullptr), m_logger(logger), 
        m_port(port), m_directory(directory), m_running(false) {}
    
    /// @brief Default deconstructor
    ~WebServer() { Stop(); }

    /// @brief Sets the port for 
    /// @param port 
    void SetPort(int port);

    /// @brief Sets the target directory for web requests
    /// @param path 
    void SetDirectory(std::string directory);

	/// @brief Configure the webserver with a port and a path
	/// @param port 
	/// @param path 
	void Configure(int port, std::string directory);

	/// @brief Starts the web server process. NOTE: This blocks the calling thread
    void Start();

    /// @brief Stops the web server process.
    void Stop();

	/// @brief handles config page request
	/// @param c - the connection for the page request
	/// @param hm - the page request data
	void HandleConfigPage(mg_connection* c);

	/// @brief handles the layout page request
	/// @param c - the connection for the page request
	/// @param bodyContent - the content to be embedded in the layout page
	void HandleLayoutPage(mg_connection* c, std::string bodyContent);

	/// @brief handles the reboot action
	/// @param c - the connection for the page request
	void HandleReboot(mg_connection* c);

protected:

private:
	/// @brief Attempts to parse a specified variable from the specified content
	/// @param name - [in] - the variable to parse
	/// @param data - [in] - the data to parse the variable from
	/// @return - the string format of the variable value
	std::string Parse(const std::string name, const char* data);

    std::string     m_name					= "";
    CivetServer*    m_server;
    LogClient&      m_logger;
    int             m_port					= 0;
    std::string     m_directory				= "";
    bool            m_running				= false;
	const mg_context* m_context				= {};
	std::string		m_listeningAddress		= "";
	char			m_buffer[BUFFER_SIZE]	= {};
	std::mutex m_connectionLock				= {};
	std::map<struct mg_connection*, bool> m_connections = {};
};